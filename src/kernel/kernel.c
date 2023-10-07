//void function() {
//  // call from main
//}

// how to link this shit method
void kmain() {
    char* video_memory = (char*)0xb8000;
    *video_memory = 'X';
}
