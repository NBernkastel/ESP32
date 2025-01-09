from fastapi import FastAPI
import psutil
import time

app = FastAPI()


@app.get("/state")
async def state():
    cpu_usage = psutil.cpu_percent(interval=1)
    cpu_count = psutil.cpu_count(logical=True)

    memory = psutil.virtual_memory()
    total_memory = memory.total / (1024 ** 3)  # В ГБ
    used_memory = memory.used / (1024 ** 3)  # В ГБ
    memory_usage = memory.percent

    disk = psutil.disk_usage('/')
    total_disk = disk.total / (1024 ** 3)  # В ГБ
    used_disk = disk.used / (1024 ** 3)  # В ГБ
    disk_usage = disk.percent

    net_io = psutil.net_io_counters()
    bytes_sent = net_io.bytes_sent / (1024 ** 2)  # В МБ
    bytes_recv = net_io.bytes_recv / (1024 ** 2)  # В МБ

    return f"""CPU: {cpu_usage}% ({cpu_count} cores)
    Memory: {used_memory:.2f}/{total_memory:.2f} GB ({memory_usage}%)
    Memory: {used_memory:.2f}/{total_memory:.2f} GB ({memory_usage}%)
    Disk: {used_disk:.2f}/{total_disk:.2f} GB ({disk_usage}%)
    Network: Sent {bytes_sent:.2f} MB, Received {bytes_recv:.2f} MB"""
