FROM debian:bookworm-slim

RUN apt-get update -qq && \
    apt-get install -y -qq gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf picolibc-riscv64-unknown-elf make clang git debianutils && \
    rm -rf /var/lib/apt/lists/* && \
    bash -c 'for f in /usr/bin/riscv64-unknown-elf-*; do ln -s "$f" "${f/riscv64/riscv32}"; done'

WORKDIR /src

CMD ["bash", "-c", "clang++ -std=c++14 tests/test_amiga_shortcuts.cpp -o test_shortcuts && ./test_shortcuts && clang++ -std=c++14 tests/test_machine_profiles.cpp -o test_profiles && ./test_profiles && cd src/MPUBIOS/app && make clean && make"]
