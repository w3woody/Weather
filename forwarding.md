# Request Forwarding.

If you need to forward a request from HTTP to HTTPS to use an Ethernet Shield which does not provide SSL/TLS support, you can use the Java servlet below. This forwards requests to the servlet provided to the DarkSky API.

You will need to compile this using Java as part of a servlet deployment and upload to a Java Web server like Tomcat.

    /*  WeatherForwardServlet.java
     * 
     *      WForward Copyright 2018 William Edward Woody, all rights reserved.
     */
    package com.chaosinmotion.wforward.server;

    import java.io.ByteArrayOutputStream;
    import java.io.IOException;
    import java.io.InputStream;
    import java.net.URL;
    import java.net.URLConnection;
    import javax.servlet.ServletException;
    import javax.servlet.ServletOutputStream;
    import javax.servlet.http.HttpServlet;
    import javax.servlet.http.HttpServletRequest;
    import javax.servlet.http.HttpServletResponse;

    /**
     * @author woody
     *
     */
    public class WeatherForwardServlet extends HttpServlet
    {
        static long throttle = 10000;       /* 10 seconds */
    
        private static final long serialVersionUID = 1L;
    
        static Object syncObject = new Object();
        static long lastCachedTime;

        @Override
        protected void doGet(HttpServletRequest req, HttpServletResponse resp)
                throws ServletException, IOException
        {
            ServletOutputStream out = resp.getOutputStream();
        
            /*
             *  Determine if we're getting requests too fast.
             */
        
            resp.setContentType("application/json");
        
            synchronized(syncObject) {
                long time = System.currentTimeMillis();
                if (time - throttle < lastCachedTime) {
                    out.println("{ \"error\": \"throttle\", \"errorid\": -1 }");
                    return;
                }
                lastCachedTime = time;
            }

            /*
             *  Pull the request and forward
             */
        
            String uri = req.getRequestURI();
            int index = uri.indexOf("api/");
            String trim = uri.substring(index+4);
        
            /*
             *  Make request
             */
        
            String request = "https://api.darksky.net/forecast/" + trim;
            URL url = new URL(request);
            URLConnection conn = url.openConnection();
            conn.setDefaultUseCaches(false);
            conn.setUseCaches(false);
            conn.setAllowUserInteraction(false);
        
            InputStream is = conn.getInputStream();
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            byte[] buffer = new byte[1024];
            int len;
            while (-1 != (len = is.read(buffer))) {
                baos.write(buffer, 0, len);
            }
            is.close();
        
            out.write(baos.toByteArray());
        }
    }
