import sort_runner
import sys

def main(filename):
    with open(filename, 'r') as f:
        A = [int(x) for x in f.readlines()]
    #sort_runner.run_algs_part1(A, filename)    #Lager separat fil for hver algoritme
    sort_runner.run_algs_part2(A, filename)

if __name__ == '__main__':
    main(sys.argv[1])

