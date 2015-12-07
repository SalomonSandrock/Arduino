void StartConversation2(EthernetClient client){
  if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    client.println();

                    // AJAX request for switch state
                    if (HTTP_req.indexOf("ajax_switch") > -1) {
                        // read switch state and analog input
                        GetAjaxData(client);
                    }
                    else {  // HTTP request for web page
                        // send web page - contains JavaScript with AJAX calls
                        client.println("<!DOCTYPE html>");
                        client.println("<html>");
                          client.println("<head>");
                            client.println("<title>Arduino Web Page</title>");
                            client.println("<script>");
                              client.println("function GetAjaxData() {");
                              client.println(
                                "nocache = \"&nocache=\" + Math.random() * 1000000;");
                              client.println("var request = new XMLHttpRequest();");
                              client.println("request.onreadystatechange = function() {");
                              client.println("if (this.readyState == 4) {");
                              client.println("if (this.status == 200) {");
                              client.println("if (this.responseText != null) {");
                              client.println("document.getElementById(\"sw_an_data\")\
                                .innerHTML = this.responseText;");
                              client.println("}}}}");
                              client.println(
                                "request.open(\"GET\", \"ajax_switch\" + nocache, true);");
                              client.println("request.send(null);");
                              client.println("setTimeout('GetAjaxData()', 1000);");
                              client.println("}");
                            client.println("</script>");
                          client.println("</head>");

                          client.println("<body onload=\"GetAjaxData()\">");
                            client.println("<h1>Arduino AJAX Input</h1>");
                            client.println("<div id=\"sw_an_data\">Switch state: Not requested...</p>");
                            client.println("</div>");
                          client.println("</body>");
                        client.println("</html>");
                    }
                    // display received HTTP request on serial port
                    Serial.print(HTTP_req);
                    HTTP_req = "";            // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                }
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

// send the state of the switch to the web browser
void GetAjaxData(EthernetClient cl)
{
    float t = dht.readTemperature();  //Temperatur auslesen
    float h = dht.readHumidity();     //Luftfeuchte auslesen

    if (lightState) {
        cl.println("<p>Lightstate: ON</p>");
    }
    else {
        cl.println("<p>Lightstate: OFF</p>");
    }

    cl.print("<p><h2>Temperature: <font color=indigo>");
    cl.print(t, 2);
    cl.println(" &deg C <font color=indigo>");
    cl.println("</font></h2></p>");

    cl.print("<p><h2>Humidity = <font color=indigo>");
    cl.print(h, 2);
    cl.print(" % <font color=indigo>");
    cl.println("</font></h2></p>");
}
