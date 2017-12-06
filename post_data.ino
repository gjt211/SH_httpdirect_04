// -----------------------------------------------------------------------------------
void post_data(String data){
  WiFiClient clienthttp;
  digitalWrite(__DATALED,_DATALEDON);

  hport = atoi(configuration.sport);

  if (! clienthttp.connect(configuration.shost, hport)) {
    printHeap();
    Serial.println(F("[HTTP] {ERR} Unable to contact host."));
    ledmode = 2;
    connect_counter++;
    return;
  } else {
    printHeap();
    Serial.print(F("[HTTP] {POST} "));
    Serial.println(data);
    ledmode = 1;
    connect_counter = 0;
  }

  yield();
  clienthttp.print("POST ");
  clienthttp.print(post_file);
  clienthttp.print(" HTTP/1.1\r\n");
  clienthttp.print("Host: ");
  clienthttp.print(String(configuration.shost));
  clienthttp.print("\r\n");
  clienthttp.print("Connection: close\r\n");
  yield();
  clienthttp.print("X-SHAPIKEY: ");
  clienthttp.print(String(configuration.sapikey));
  clienthttp.print("\r\n");
  yield();
  clienthttp.print("Content-Type: application/x-www-form-urlencoded\r\n");
  clienthttp.print("Content-Length: ");
  clienthttp.print(data.length());
  clienthttp.print("\r\n\r\n");
  clienthttp.print(data);
  clienthttp.stop();

  digitalWrite(__DATALED,_DATALEDOFF);
  
}

// -----------------------------------------------------------------------------------
boolean post_http_data(String command, String value, String pre, String post){
  String data = "cmd=" + command + "&id=" + dev_key + "&t1=" + value + "&pre=" + pre + "&post=" + post;

  WiFiClient clienthttp;
  digitalWrite(__DATALED,_DATALEDON);

  hport = atoi(configuration.sport);

  if (! clienthttp.connect(configuration.shost, hport)) {
    printHeap();
    Serial.println(F("[HTTP] {ERR} Unable to contact host."));
    ledmode = 2;
    connect_counter++;
    return false;
  } else {
    printHeap();
    Serial.print(F("[HTTP] {POST} "));
    Serial.println(data);
    ledmode = 1;
    connect_counter = 0;
  }

  yield();
  clienthttp.print("POST ");
  //clienthttp.print(post_file);
  clienthttp.print(post_file);
  clienthttp.print(" HTTP/1.1\r\n");
  clienthttp.print("Host: ");
  clienthttp.print(String(configuration.shost));
  clienthttp.print("\r\n");
  clienthttp.print("Connection: close\r\n");
  yield();
  clienthttp.print("X-SHAPIKEY: ");
  clienthttp.print(String(configuration.sapikey));
  clienthttp.print("\r\n");
  yield();
  clienthttp.print("Content-Type: application/x-www-form-urlencoded\r\n");
  clienthttp.print("Content-Length: ");
  clienthttp.print(data.length());
  clienthttp.print("\r\n\r\n");
  clienthttp.print(data);
  clienthttp.stop();

  digitalWrite(__DATALED,_DATALEDOFF);
  return true;
  
}
