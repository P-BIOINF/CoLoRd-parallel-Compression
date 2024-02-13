import psutil

def get_memory_usage(process_name):
    for proc in psutil.process_iter(['name', 'memory_info']):
        if proc.info['name'] == process_name:
            return proc.info['memory_info'].rss
    return None

process_name = "CoLoRd parallel decompression.exe" # Nazwa programu C++ -> CoLoRd parallel decompression.exe
memory_usage = get_memory_usage(process_name)
if memory_usage is None:
    print(f"Nie można znaleźć procesu o nazwie '{process_name}'\n")
else:
    print(f"Zużycie pamięci przez proces '{process_name}': {memory_usage / (1024 * 1024)} MB\n")