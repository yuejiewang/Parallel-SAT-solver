import unittest
import os
from check_sat import *

input_dir = "../input/"
par_output_dirs = ["../output_parallel_p0/", "../output_parallel_p1/", "../output_parallel_p2/",
                   "../output_parallel_p3/", "../output_parallel_p4/"]
seq_output_dir = "../sequential_output/"

input_files = os.listdir(input_dir)
sat_input_files = [file for file in input_files if file[0] == "s"]
unsat_input_files = [file for file in input_files if file[0] == "u"]


class OutputCorrectnessTests(unittest.TestCase):
    def test_one_sequential_sat(self):
        valid = check_correctness(input_dir+"sat_fromAIclass.cnf", seq_output_dir+"sat_fromAIclass.txt")
        self.assertEqual(True, valid)

    def test_one_sequential_unsat(self):
        valid = check_correctness(input_dir+"unsat_v5_c3-44_0.cnf", seq_output_dir+"unsat_v5_c3-44_0.txt")
        self.assertEqual(False, valid)

    def test_one_parallel_sat(self):
        valid = check_correctness(input_dir+"sat_fromAIclass.cnf", par_output_dirs[1]+"sat_fromAIclass.txt")
        self.assertEqual(True, valid)

    def test_one_parallel_unsat(self):
        valid = check_correctness(input_dir+"unsat_hole6.cnf", par_output_dirs[1]+"unsat_hole6.txt")
        self.assertEqual(False, valid)

    def test_all_sequential_sat(self):
        # working for all but par8-1-c.cnf
        for file in sat_input_files:
            base = file[0:-4]
            valid = check_correctness(input_dir+file, seq_output_dir+base+".txt")
            self.assertEqual(True, valid, file)

    def test_all_sequential_unsat(self):
        # working for all but unsat_hole6.cnf
        for file in unsat_input_files:
            base = file[0:-4]
            valid = check_correctness(input_dir+file, seq_output_dir+base+".txt")
            self.assertEqual(False, valid, file)

    def test_all_parallel_sat(self):
        for par_output_dir in par_output_dirs:
            for file in sat_input_files:
                base = file[0:-4]
                valid = check_correctness(input_dir+file, par_output_dir+base+".txt")
                self.assertEqual(True, valid, file)

    def test_all_parallel_unsat(self):
        for par_output_dir in par_output_dirs:
            for file in unsat_input_files:
                    base = file[0:-4]
                    valid = check_correctness(input_dir+file, par_output_dir+base+".txt")
                    self.assertEqual(False, valid, file)


if __name__ == '__main__':
    unittest.main()
