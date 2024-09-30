import serial
import time

# PC to MCU transmission
def send_string(message):
    total_bits = 0
    start_time = time.time()  
    for char in message:
        ser.write(char.encode())  
        # time.sleep(0.05)          # delay for each character transmission
        total_bits += 8  
        end_time = time.time()
        transmission_time = end_time - start_time
        if transmission_time > 0:
            bps = total_bits / transmission_time  # Calculate bits per second
            print(f"Bits per second (bps) during transmission: {bps:.2f}")

# MCU to PC  via UART
def receive_bytes_and_bps():
    received_data = []
    total_bits = 0
    start_time = time.time()  

    while True:
        data = ser.read()  
        if not data:
            continue    # wait for data
        char_value = data.decode('ascii') 
        
        print(f"Received byte: ('{char_value}')")  # Print byte 
        
        received_data.append(char_value)
        total_bits += 8  # Each byte is 8 bits 

        if char_value == '\x00':  
            break

    received_string = ''.join(received_data[:-1])  # null char exclude
    print(f"Received string: '{received_string}'")  # Print the received string

    end_time = time.time()
    transmission_time = end_time - start_time
    if transmission_time > 0:
        bps = total_bits / transmission_time  # bps calc
        print(f"Bits per second (bps) during reception: {bps:.2f}")

    return received_data

# Init Uart comm
ser = serial.Serial(
    port='COM8',        
    baudrate=2400,             
    bytesize=serial.EIGHTBITS,
    stopbits=serial.STOPBITS_TWO,  
    parity=serial.PARITY_NONE,
    timeout=1           
)

# for resetting and initializing the Atmega328p
time.sleep(2)

# String to send to arduino uno
message = """Finance Minister Arun Jaitley Tuesday hit out at former RBI governor Raghuram Rajan for predicting that the next banking crisis would be triggered by MSME lending, saying postmortem is easier than taking action when it was required. Rajan, who had as the chief economist at IMF warned of impending financial crisis of 2008, in a note to a parliamentary committee warned against ambitious credit targets and loan waivers, saying that they could be the sources of next banking crisis. Government should focus on sources of the next crisis, not just the last one. 

In particular, government should refrain from setting ambitious credit targets or waiving loans. Credit targets are sometimes achieved by abandoning appropriate due diligence, creating the environment for future NPAs," Rajan said in the note." Both MUDRA loans as well as the Kisan Credit Card, while popular, have to be examined more closely for potential credit risk. Rajan, who was RBI governor for three years till September 2016, is currently.\0""" 


print("Sending data to MCU...")
print(message)
#time.sleep(2) #adding delay inorder to see the message
send_string(message)

# providing delay before retrieve the data from EEPROM
time.sleep(2)

print("Receiving data from MCU...")
received_message = receive_bytes_and_bps()

ser.close()
