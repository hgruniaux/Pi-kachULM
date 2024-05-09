#include "kernel.hpp"
#include "hardware/device.hpp"
#include "hardware/interrupts.hpp"
#include "hardware/uart.hpp"

#include <libk/log.hpp>
#include "boot/kernel_dt.hpp"
#include "dtb/node.hpp"
#include "hardware/timer.hpp"

void print_property(const Property p) {
  if (p.is_string()) {
    LOG_INFO("DeviceTree property {} (size: {}) : '{}'", p.name, p.length, p.data);
  } else if (p.length % sizeof(uint32_t) == 0) {
    LOG_INFO("DeviceTree property {} (size: {}) :", p.name, p.length);
    const auto* data = (const uint32_t*)p.data;
    for (size_t i = 0; i < p.length / sizeof(uint32_t); i++) {
      LOG_INFO("  - At {}: {:#x}", i, libk::from_be(data[i]));
    }
  } else {
    LOG_INFO("DeviceTree property {} (size: {}) :", p.name, p.length);
    for (size_t i = 0; i < p.length; i++) {
      LOG_INFO("  - At {}: {:#x}", i, (uint8_t)p.data[i]);
    }
  }
}

void find_and_dump(libk::StringView path) {
  Property p;

  if (!KernelDT::find_property(path, &p)) {
    LOG_CRITICAL("Unable to find property: {}", path);
  }

  print_property(p);
}

void dump_current_el() {
  switch (get_current_exception_level()) {
    case ExceptionLevel::EL0:
      LOG_INFO("CurrentEL: EL0");
      break;
    case ExceptionLevel::EL1:
      LOG_INFO("CurrentEL: EL1");
      break;
    case ExceptionLevel::EL2:
      LOG_INFO("CurrentEL: EL2");
      break;
    case ExceptionLevel::EL3:
      LOG_INFO("CurrentEL: EL3");
      break;
  }
}

extern "C" void init_interrupts_vector_table();

[[noreturn]] void kmain() {
  UART u0(UART::Id::UART0, 2000000);  // Set to a High Baud-rate, otherwise UART is THE bottleneck :/

  libk::register_logger(u0);

  libk::set_log_timer([]() { return GenericTimer::get_elapsed_time_in_ms(); });

  dump_current_el();
  init_interrupts_vector_table();

  LOG_INFO("Kernel built at " __TIME__ " on " __DATE__);

  // Setup DeviceTree
  KernelMemory::init();

  // Setup Device
  if (!Device::init()) {
    libk::halt();
  }

  {
    uint64_t tmp;
    asm volatile("mov %x0, sp" : "=r"(tmp));
    LOG_INFO("SP : {:#x}", tmp);
  }

  LOG_INFO("Board model: {}", KernelDT::get_board_model());
  LOG_INFO("Board revision: {:#x}", KernelDT::get_board_revision());
  LOG_INFO("Board serial: {:#x}", KernelDT::get_board_serial());
  LOG_INFO("Temp: {} °C / {} °C", Device::get_current_temp() / 1000, Device::get_max_temp() / 1000);
  LOG_INFO("Uart address: {:#x}", KernelDT::get_device_mmio_address("uart0"));

  LOG_INFO("Memory overhead: {:#x}", KernelMemory::get_memory_overhead());

  MemoryPage page;
  LOG_INFO("Allocate Page: {}", KernelMemory::new_page(&page));

  const char* data = "Hello World!";

  LOG_INFO("Page Write: {}", page.write(4090, data, sizeof(data)));

  char buffer[1024];
  const size_t nb_read = page.read(4090, buffer, sizeof(data));

  LOG_INFO("Page Read: {}", nb_read);
  LOG_INFO("Read: {:$}", libk::StringView(buffer, nb_read));

  page.free();

  LOG_INFO("Heap test start:");

  char* heap_start = (char*)KernelMemory::get_heap_end();
  LOG_INFO("Current kernel end: {:#x}", (uintptr_t)heap_start);

  constexpr const char* m_data = "Hello Kernel Heap!";
  constexpr size_t m_data_size = libk::strlen(m_data);
  LOG_INFO("New kernel end: {:#x}", KernelMemory::change_heap_end(m_data_size));

  libk::memcpy(heap_start, m_data, m_data_size);
  LOG_INFO("Written: {:$}", heap_start);

  //  SyscallManager::get().register_syscall(24, [](Registers& ) { LOG_INFO("Syscall 24"); });
  //
  //  //  Enter userspace
  //  jump_to_el0();
  //  asm volatile("mov w8, 24\n\tsvc #0");
  //  libk::halt();

  //  FrameBuffer& framebuffer = FrameBuffer::get();
  //  if (!framebuffer.init(640, 480)) {
  //    LOG_CRITICAL("failed to initialize framebuffer");
  //  }
  //
  //  const uint32_t fb_width = framebuffer.get_width();
  //  const uint32_t fb_height = framebuffer.get_height();
  //
  //  graphics::Painter painter;
  //
  //  const char* text = "Hello kernel World from Graphics!";
  //  const PKFont font = painter.get_font();
  //  const uint32_t text_width = font.get_horizontal_advance(text);
  //  const uint32_t text_height = font.get_char_height();
  //
  //   Draw the text at the middle of screen
  //  painter.clear(graphics::Color::WHITE);
  //  painter.set_pen(graphics::Color::BLACK);
  //  painter.draw_text((fb_width - text_width) / 2, (fb_height - text_height) / 2, text);
  //
  LOG_ERROR("END");
  while (true) {
    u0.write_one(u0.read_one());
  }
}
