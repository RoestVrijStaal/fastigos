void gdt_init(void);
void idt_init(void);
void install_idt_handler(uint8_t index, uint32_t handler);
