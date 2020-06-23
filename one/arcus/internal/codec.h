// Todo: Encoding & decoding to/from the Arcus byte data.

// From one::Message to (char*, size_t)
// 1. detect static Arcus protocol prefix 4-byte "ARC\0"
// 2. Reader static header struct
// 3. validate
// 4. convert payload, if any, from (char*, size_t) to one::Payload.
// 5. Configure and return message.
// 6. Error handling throughout.

// From (char*, size_t) to one::Message
// 1. Create output header struct.
// 2. Set opcode.
// 3. Serialize Payload to char*, size_t
// 4. Error handling throughout.
