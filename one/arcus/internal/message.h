namespace one {

// An array value that can be contained in a message's data.
class Array final {

};

// An object value that can be contained in a message's data.
class Object final {

};

class Message final {
    int set_code(int code);
    int code();

    // Getters.
    int  val_int(const char * key, int* error);
    const char * val_string(const char * key, int* error);
    Array&  val_array(const char * key, int* error);
    Object&  val_object(const char * key, int* error);

    // Setters.
    void set_val_int(const char * key, int val, int* error);
    void set_val_string(const char * key, const char* val, int* error);
    void set_val_array(const char * key, const Array & val, int* error);
    void set_val_object(const char * key, const Object & val, int* error);
};

}