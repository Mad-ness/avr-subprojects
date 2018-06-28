# API

The API which provides the proxy subsystem. It serves the requests to itself and also the requests which are intended to the remote board, behind RF24L01 module.

There are two types of API:
- proxy API 
- device API

The both ones are served the single module - proxy.

The **Proxy API** requests start from ***/proxy/...*** part in URL, the **Device API** starts with ***/device/...*** string. All requests are sent using the GET requests (if not mentioned other) and use or may not use parameters.

Some API function respond with JSON values, some may not, it depends.


Along with requested data, other parameters are also provided such as following:
```json
    {
        "data": {
            <requested data>
        },
        "msg": "Information message about operation, or an error message, if any",
        "request_uri": "URI that has been requested, the part after the domain name",
        "status_code": "HTTP code like 200, 404, ..."
    }
```

Below in the API details, the **Return data** output is put down inside the ***data*** block.

## Proxy API

### /proxy/ping
**Description**: asks the proxy "Are you alive?"\
**Method**: GET\
**Return code**: 20x, if alive\
**Return data**: none


### /proxy/uptime
**Description**: Asks the proxy's uptime, in seconds\
**Method**: GET\
**Return code**: 20x, if Ok\
**Return data**: 
  - uptime: positive number


## Device API

Almost (or all) Device API function use parameters that should be sent along with requests. If some or all parameters are missed, the proxy responds with information about missing parameters in the **msg**.

Device API is a low-level API that redirects to the remote board over RF24L01 module, so getting responses back may take a while. In order to build an asynchronous model, the following approarch is implemented. When a function is called, it is put in the **Input queue**.  
The proxy periodically ( 1/10 ms but may be changed) checks this queue on new requests and sends them to the remote board, one at once. The proxy also checks the responses from the remote board and updates the **Input queue** with reponses.

In order to get requested data, one should send again the same requests with the same parameters after a while.

If you have sent the same multiple requests and the remote board response took a long time, the **Input queue** may have multiple the same requests. If the board reponses on any of them, all of the awaiting requests will be refreshed with that data.

### /device/ping
**Description**:
**Method**: GET\
**Parameters**: did=<int> - device ID
**Return code**: 20x, if the request accepted by the proxy
**Return data**: none

