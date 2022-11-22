#!/bin/bash
# Copyright 2018 Jacob Lifshay
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# 32KiB
memory_size=$((32<<10))
# 16kbit
block_size_log_2=11

output_file=""

function open_output_file()
{
    output_file="$1"
    exec >"$1"
}

function error()
{
    echo "error:" "$@" >&2
    if [[ "" != "$output_file" ]]; then
        rm "$output_file"
    fi
    exit 1
}

chunk_size_log_2=$((block_size_log_2+2))
chunk_size=$((1<<chunk_size_log_2))
chunk_count=$((memory_size / chunk_size))
((memory_size % chunk_size == 0)) || error "memory_size must be a multiple of chunk_size"

if [[ "$1" == "-v" ]]; then
    open_output_file block_memory.v
    cat <<EOF
/*
 * Copyright 2018 Jacob Lifshay
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
\`timescale 1ns / 1ps

module block_memory(
    input clk,
    input [31:0] a_ram_address,
    input [3:0] a_write_enable,
    input [31:0] a_write_input,
    output reg [31:0] a_read_output,
    input [31:0] b_ram_address,
    output reg [31:0] b_read_output
    );
EOF

    for((i = 0; i < chunk_count; i++)); do
        cat <<EOF

    wire a_enable_${i} = a_ram_address[31:${block_size_log_2}] == ${i};
    wire b_enable_${i} = b_ram_address[31:${block_size_log_2}] == ${i};
    wire [3:0] a_write_enable_${i} = {4{a_enable_${i}}} & a_write_enable;
    wire [31:0] a_read_output_${i};
    wire [31:0] b_read_output_${i};
EOF
        for byte in {0..3}; do
            cat <<EOF
    block_memory_16kbit #(
        .initial_file("software/ram_${i}_byte${byte}.hex")
        ) ram_${i}_byte${byte}(
        .clk(clk),
        .port_a_address(a_ram_address[$((block_size_log_2 - 1)):0]),
        .port_a_write_enable(a_write_enable_${i}[${byte}]),
        .port_a_write_input(a_write_input[$((byte * 8 + 7)):$((byte * 8))]),
        .port_a_read_output(a_read_output_${i}[$((byte * 8 + 7)):$((byte * 8))]),
        .port_b_address(b_ram_address[$((block_size_log_2 - 1)):0]),
        .port_b_read_output(b_read_output_${i}[$((byte * 8 + 7)):$((byte * 8))])
        );

EOF
        done
    done

    cat <<EOF

    always @* begin
        case(a_ram_address[31:${block_size_log_2}])
EOF

    for((i = 0; i < chunk_count; i++)); do
    cat <<EOF
        ${i}: a_read_output = a_read_output_${i};
EOF
    done

    cat <<EOF
        default: a_read_output = 32'hXXXXXXXX;
        endcase
    end

    always @* begin
        case(b_ram_address[31:${block_size_log_2}])
EOF

    for((i = 0; i < chunk_count; i++)); do
    cat <<EOF
        ${i}: b_read_output = b_read_output_${i};
EOF
    done

    cat <<EOF
        default: b_read_output = 32'hXXXXXXXX;
        endcase
    end
endmodule
EOF
    exit 0
elif [[ "$1" == "-s" ]]; then
    open_output_file generate_hex_files.sh
    chmod +x generate_hex_files.sh
    cat <<EOF
#!/bin/bash
# Copyright 2018 Jacob Lifshay
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

mapfile -t words < <(hexdump -v -e '/4 "%08X\n"' < ram.bin)
chunk_size=${chunk_size}
chunk_count=${chunk_count}
EOF
    cat <<'EOF'
if (( chunk_size * chunk_count != 4 * ${#words[@]} )); then
    echo "ram.bin is the wrong size: expected $((chunk_size * chunk_count / 4)) words, got ${#words[@]}" >&2
    exit 1
fi
word_index=0
for((chunk=0;chunk<chunk_count;chunk++)); do
    exec 3>"ram_${chunk}_byte0.hex" 4>"ram_${chunk}_byte1.hex" 5>"ram_${chunk}_byte2.hex" 6>"ram_${chunk}_byte3.hex"
    for((i=0;i<chunk_size;i+=4)); do
        word="${words[word_index++]}"
        echo "${word:6:2}" >&3
        echo "${word:4:2}" >&4
        echo "${word:2:2}" >&5
        echo "${word:0:2}" >&6
    done
done
exit 0
EOF
else
    error $'unknown option\nusage: '"$0"$' [-v|-s]\n-v\tgenerate block_memory.v\n-s\tgenerate generate_hex_files.sh'
fi

