import os, sys

def main():

    if len(sys.argv) != 2:
        print("Usage: python3 trace.py <path to trace>")
        return

    log = ""

    path = sys.argv[1]
    base = os.path.basename(path)
    base_split = base.split('.')
    if base_split[1] != "trace":
        print(f"{path} is not a .trace file")
        return
    name = base_split[0]

    main_trace = open(path, "r")

    current_dir = os.getcwd()
    trace_dir = os.path.join(current_dir, f"{name}_traces")
    if not os.path.exists(trace_dir):
        os.makedirs(trace_dir)
    
    lines = main_trace.readlines()

    loads = 0
    stores = 0
    threads = dict()


    for line in lines:
        tokens = line.split(' ')

        if tokens[0] == "t0":
            continue
        elif "ld" in line:
            loads += 1
        elif "st" in line:
            stores += 1
        else:
            continue

        thread = tokens[0]
        instr = tokens[1]
        addr = tokens[2]

        if thread not in threads:
            thread_path = os.path.join(trace_dir, f"{name}_{thread}.trace")
            thread_trace = open(thread_path, "w")
            threads[thread] = thread_trace

        threads[thread].write(f"{instr} {addr}")


if __name__ == "__main__":
    main()