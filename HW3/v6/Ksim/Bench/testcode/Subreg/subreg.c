int main() {
    char buffer[8] = {0xaa, 0xaa, 0xbb, 0xcc, 0xdd, 0x11, 0x22, 0x33};
    
    unsigned int val = 0xaaaabbcc;
    unsigned int loaded = 0;
    

    
    memcpy(&loaded, buffer + 1, sizeof(int));

    printf("Original: 0x%08X\n", val);
    printf("Loaded  : 0x%08X\n", loaded);

    return 0;
}

