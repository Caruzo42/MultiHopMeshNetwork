# Interface Specification

This document describes the protocol used for communication over the mesh network defined in the 'Radiohead' library (http://www.airspayce.com/mikem/arduino/RadioHead/index.html?utm_source=platformio&utm_medium=piohome) by Mike McCauley.

## 1. Security

The current version of the protocol does not include any security measures at the network layer. Therefore, anyone can potentially use the existing Radiohead network for communication over wider distances. However, on the application layer, nodes should be authenticated by the gateway upon sending their initial CONNECT request.

## 2. Topics

The protocol supports the following topics:

### 2.1 `v1/backend/measurements`

The payload for this topic includes the timestamp, sender UUID, and content of type and value. The payload is structured as follows:

```json
{
    "timestamp": <unix-timestamp>,
    "sender_uuid": <integer>,
    "content": {
        "type": <string>,
        "value": <string>
    }
}
```

### 2.2 `v1/updates/missing`

The payload for this topic includes the index of the missing block. The payload is structured as follows:

```json
{
    "content": {
        "missingBlockIndex": <int>
    }
}
```

## 3. OTA-Updates

Over-the-air (OTA) updates should be initiated by the Gateway (Node 0). Upon receiving an update packet, nodes in the network should broadcast said packet exactly once. If there are packets missing, the node should request those packets using the relevant topic.

## 4. Packet Structure

The Packet structure in this protocol is similar to MQTT and is composed of the following sections:

### 4.1 Fixed Header (2 Bytes)
The fixed header consists of the Control Packet Type (4 Bit), Control Flags (4 Bit), and the Packet Length (1 Byte, including the header and payload).

#### 4.1.1 Control Packet Type

The control packet type specifies the type of packet being sent. The available packet types are as follows:

| Name       | Value | Direction                       | Description                              | Usage in WaldWW           | Variable Header  | Payload          |
|------------|:-----:|---------------------------------|------------------------------------------|---------------------------|------------------|------------------|
| CONNECT    | 1     | Node -> Gateway                 | Connection Request                       | Initial Connection        | [Required](#L81) | None             |
| CONNACK    | 2     | Gateway -> Node                 | Connection Acknowledgement               | Initial Connection        | [Required](#L90) | None             |
| PUBLISH    | 3     | Node -> Gateway Gateway -> Node | Publish Packet                           | Health-Ping, Measurements | [Required](#L109) | [Required](#L102)|
| PUBACK     | 4     | Node -> Gateway Gateway -> Node | Publish Acknowledgement (QoS Lvl 1 only) | Measurements              | [Required](#L119) | None             |
| SUBSCRIBE  | 8     | Node -> Gateway                 | Subscribe Request                        | None                      | [Required](#L127) | None             |
| SUBACK     | 9     | Gateway -> Node                 | Subscribe Acknowledgement                | None                      | [Required](#L137) | None             |
| DISCONNECT | 14    | Node -> Gateway                 | Disconnect Information                   | After Update              | [Required](#L145)| None             |

#### 4.1.2 Control Flags

The control flags are used to control packet transmission. They are as follows:

| Bits | Name   | Description                                                                        |
|:----:|--------|------------------------------------------------------------------------------------|
| 1    | DUP    | Duplicate publish control packet. If DUP=1 the client has sent the packet before.  |
| 2    | QOS    | QoS Level. Only Level 0 and 1 supported for now.                                   |
| 1    | RETAIN | Not implemented.                                                                   |

### 4.2 Variable Header

The structure of the variable header is dependent on the packet type. The structures for each packet type are described below:

Certainly, here are more detailed specifications for each variable header based on the packet type:

#### 4.2.1 CONNECT

The variable header for the CONNECT packet contains the following information:

| Bytes | Name             | Description                           |
|:----:|------------------|----------------------------------------|
| 1    | Protocol Version | Protocol Version Number. 0x01 for now. |
| 16   | UUID             | The UUID associated with the ESP32.    |

#### 4.2.2 CONNACK

The variable header for the CONNACK packet includes the following data:

| Bytes | Name                | Description                                                              |
|:-----:|---------------------|--------------------------------------------------------------------------|
|   1   | Connect Return code | Return Code responding to the connection request. Defined below.         |
|   1   | Network ID          | New ID assigned to the connecting node for communication in the network. |
| 16    | UUID                | The UUID of the Node the network ID has been assigned to.                |

The Connect Return code can take one of the following values:

| Value    |     Return Code   Response                                    |     Description                                                                             |
|:--------:|---------------------------------------------------------------|---------------------------------------------------------------------------------------------|
|     0    |     0x00 Connection Accepted                                  |     Connection accepted.                                                                    |
|     1    |     0x01 Connection Refused, unacceptable protocol version    |     The Server does not support the level of the protocol requested by the Client.          |
|     3    |     0x03 Connection Refused, Server unavailable               |     The Network Connection has been made but the server is unavailable.                     |
|     5    |     0x05 Connection Refused, not authorized                   |     The Client is not authorized to connect.                                                |

#### 4.2.3 PUBLISH

The variable header for the PUBLISH packet includes the following fields:

| Bytes | Content              | Description                                                       |
|:-----:|----------------------|-------------------------------------------------------------------|
| 1     | Length of Topic Name | The length of the following topic name in Bytes.                  |
| ?     | Topic Name           | The name of the topic on which to publish the packet (in UTF-8).  |
| 2     | Packet ID            | The unique identifier of the packet.                               |

#### 4.2.4 PUBACK

The variable header for the PUBACK packet contains the Packet-ID of the packet being acknowledged:

| Bytes | Content              | Description                                                       |
|:-----:|----------------------|-------------------------------------------------------------------|
| 2     | Packet ID            | The unique identifier of the approved packet.                     |

#### 4.2.5 SUBSCRIBE

The variable header for the SUBSCRIBE packet contains the following fields:

| Bytes | Content              | Description                                                       |
|:-----:|----------------------|-------------------------------------------------------------------|
| 1     | Length of Topic Name | The length of the following topic name in Bytes.                  |
| ?     | Topic Name           | The name of the topic to subscribe to (in UTF-8).                 |
| 2     | Packet ID            | The unique identifier of the packet.                              |

#### 4.2.6 SUBACK

The variable header for the SUBACK packet contains the Packet-ID of the packet being acknowledged:

| Bytes | Content              | Description                                                       |
|:-----:|----------------------|-------------------------------------------------------------------|
| 2     | Packet ID            | The unique identifier of the approved subscribe-packet.           |

#### 4.2.7 DISCONNECT

The variable header for the DISCONNECT packet contains the UUID of the disconnecting device:

| Bytes | Content | Description                           |
|-------|---------|---------------------------------------|
| 16    | UUID    | The UUID of the disconnecting device. |


### 4.3 Payload

The payload structure is relevant only for the PUBLISH control packet. It contains the content of the published message in JSON format.
