# Secret Santa Project

Pair-based project with [@tamesjam](https://github.com/tamesjam) conducted during end of 6-week C-training bootcamp to consolidate learning across all topics but specifically focused on communication and client-server socket programming using either TCP or UDP.

### Requirements
- The task is to write a client-server application.
- The client must be able to connect to a running server and register a participant's name to be entered into the secret santa draw
- The server must keep a record of all registered participants
- The client must be able to trigger the drawing of the names in the server
- The server must be able to receive a command to begin drawing names
  - Upon receiving the command, the server must generate a list to assign "secret santas" to "giftees"
  - Once the draw process has begun, no more names can be registered, and no more draws can take place
- If a draw has already taken place, the client must be able to fetch the name of their assigned "giftee"
- The secret santa list must be a list that records every participant as the "secret santa" of another ("the giftee").
  - Every participant must be a "giftee" of a "secret santa"
  - Every participant must be a "secret santa" to a "giftee"
  - No "giftee" can share the same "secret santa", and likewise, no "secret santa" can share the same "giftee"
- An API must be designed to specify the communication protocol between client and server
  - The protocol is critical for this application as your client and server need to explain how they are going to communicate with each other before development begins
  - Needs to include critical information such as how the client and server will communicate, the size of these messages, and any additional information you feel is important
