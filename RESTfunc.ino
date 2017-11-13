/*
   Based on HTTP REST function posted on Adafruit/AIO Forum by user 'hberg32'.
   Forum post can be found at: https://forums.adafruit.com/viewtopic.php?f=56&t=123666#p622065
   No particular license is known or implied.
*/

int queryFeed(char *buf, char *feed) {
  while (io.status() < AIO_CONNECTED) { }
  HTTPClient http;
  http.useHTTP10(true);//avoid chunking
  String url = "http://" + String(api_host) + String(feed_url) + String(feed);
  int httpCode;
  url += "/data/last";
  Serial.print(F("Sending URL: "));
  Serial.println(url);
  http.begin(url);
  http.addHeader("x-aio-key", IO_KEY);
  http.addHeader("content-type", "application/json");
  httpCode = http.GET();
  //Serial.print(F("httpCode: "));
  //Serial.println(httpCode);
  if (httpCode != HTTP_CODE_OK) {
    http.end();
    Serial.printf(buf, "HTTP Error %d", httpCode);
    return -1;
  } else {
    StaticJsonBuffer<jsonBufferSize> jsonBuffer;
    //DynamicJsonBuffer jsonBuffer(jsonBufferSize);
    JsonObject& json = jsonBuffer.parseObject(http.getStream());
    if (!json.success()) {
      strcpy(buf, "Parse failed");
      http.end();
      return -1;
    } else {
      strcpy(buf, json["value"]);
      http.end();
      return 0;
    }
  }
}


