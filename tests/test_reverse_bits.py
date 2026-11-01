import importlib.util
import os
import subprocess
import sys
import tempfile
import unittest

# Import reverse-bits.py dynamically since its filename contains a hyphen
script_path = os.path.join(os.path.dirname(__file__), "..", "utils", "reverse-bits.py")
spec = importlib.util.spec_from_file_location("reverse_bits", script_path)
reverse_bits = importlib.util.module_from_spec(spec)
sys.modules["reverse_bits"] = reverse_bits
spec.loader.exec_module(reverse_bits)


class TestReverseBits(unittest.TestCase):
    def test_lookup_table_completeness(self):
        """Verify that REV_TABLE contains exactly 256 bytes and inverts bit order."""
        self.assertEqual(len(reverse_bits.REV_TABLE), 256)
        for b in range(256):
            expected = sum(((b >> i) & 1) << (7 - i) for i in range(8))
            self.assertEqual(reverse_bits.REV_TABLE[b], expected)

    def test_known_patterns(self):
        """Test specific bit patterns."""
        cases = [
            (0b00000001, 0b10000000),
            (0b10000000, 0b00000001),
            (0b10100000, 0b00000101),
            (0b11001100, 0b00110011),
            (0b11110000, 0b00001111),
            (0b11111111, 0b11111111),
            (0b00000000, 0b00000000),
        ]
        for src, expected in cases:
            self.assertEqual(reverse_bits.REV_TABLE[src], expected)

    def test_idempotence(self):
        """Reversing bits twice should yield the original bytes."""
        sample = bytes(range(256))
        reversed_once = sample.translate(reverse_bits.REV_TABLE)
        reversed_twice = reversed_once.translate(reverse_bits.REV_TABLE)
        self.assertEqual(reversed_twice, sample)

    def test_cli_execution_with_empty_and_large_files(self):
        """Test script execution end-to-end via CLI subprocess."""
        with tempfile.TemporaryDirectory() as tmpdir:
            # 1. Empty file
            empty_in = os.path.join(tmpdir, "empty_in.bin")
            empty_out = os.path.join(tmpdir, "empty_out.bin")
            with open(empty_in, "wb") as f:
                pass
            res = subprocess.run(["python3", script_path, empty_in, empty_out], capture_output=True)
            self.assertEqual(res.returncode, 0)
            self.assertEqual(os.path.getsize(empty_out), 0)

            # 2. Multi-chunk binary file (>64KB)
            data_in = os.path.join(tmpdir, "data_in.bin")
            data_out = os.path.join(tmpdir, "data_out.bin")
            payload = bytes(b % 256 for b in range(150000))  # ~150KB
            with open(data_in, "wb") as f:
                f.write(payload)

            res = subprocess.run(["python3", script_path, data_in, data_out], capture_output=True)
            self.assertEqual(res.returncode, 0)

            with open(data_out, "rb") as f:
                out_content = f.read()

            expected_content = payload.translate(reverse_bits.REV_TABLE)
            self.assertEqual(out_content, expected_content)

    def test_fpga_bitstream_match(self):
        """Verify against repo's actual FPGA bitstream files if present."""
        rbf_path = "src/fpga/output_files/ap_core.rbf"
        expected_rbf_r_path = "src/fpga/output_files/bitstream.rbf_r"

        if os.path.exists(rbf_path) and os.path.exists(expected_rbf_r_path):
            with open(rbf_path, "rb") as f:
                rbf_data = f.read()
            with open(expected_rbf_r_path, "rb") as f:
                expected_data = f.read()

            actual_data = rbf_data.translate(reverse_bits.REV_TABLE)
            self.assertEqual(actual_data, expected_data)


if __name__ == "__main__":
    unittest.main()
