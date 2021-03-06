import io
import getopt
import sys


def check_correctness(cnf_file_name, result_file_name):
    cnf_file = open(cnf_file_name, "r")
    result_file = open(result_file_name)
    atoms = list()
    correct = True

    for value in result_file.readlines():
        value = value.strip()
        if value.lower() == "no solution":
            # print("no solution")
            correct = False
            break
        atoms.append(value)

    if correct:
        for clause in cnf_file.readlines():
            clause = clause.strip()
            if clause.__len__() == 0:
                continue
            if clause[0].isdigit() or clause[0] == '-':
                clause_sat = False
                for literal in clause.split(" "):
                    if atoms.count(literal) != 0:
                        clause_sat = True
                        break

                if not clause_sat:
                    correct = False
                    # print("wrong answer")
                    break

    # if correct:
    #     print("correct")

    cnf_file.close()
    result_file.close()
    return correct


def get_filenames(argv):
    try:
        options, args = getopt.getopt(argv, "i:o:c:", ["input=", "output=", "cnf="])
    except getopt.GetoptError:
        sys.exit()
    for option, value in options:
        if option in ("-o", "--output"):
            result_file_name = "output/" + value
        if option in ("-c", "-i", "--cnf", "--input"):
            cnf_file_name = value
    return cnf_file_name, result_file_name


if __name__ == '__main__':
    cnf, result = get_filenames(sys.argv[1:])
    print(check_correctness(cnf, result))
