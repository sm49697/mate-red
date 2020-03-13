import serial, re

class SerialParser:
    """
    SerialParser uses pyserial to pull lines from the serial.
    During setup, it scrapes all the servos and motors and their pins and 
    names, then returns it. After setup, it is used for parsing the 
    output of the serial's motors and servos, and generating a list of 
    the outputs.
    """
    SERIAL_PORT = None
    SERIAL_BAUDRATE = None
    ser = None
    motors_servos_dict = []
    ready_to_read = False

    def __init__(self):
        open_serial()

    def open_serial():
        """
        Uses pyserial to open a serial connection with the port and baudrate
        taken from the config. During setup, it parses the motors and servos
        into a list. Once setup is finished, the serial is ready to read.
        Called during initialization.
        """
        try:
            read_config()
            ser = serial.Serial(SERIAL_PORT, SERIAL_BAUDRATE)
            add_motors_servos()
            complete_setup()
            ready_to_read = True
        except:
            print("Could not initialize serial")

    def read_config(self):
        """
        The serial port and baudrate is stored in 'config.txt'. This method
        is used to get the update the port and baudrate.
        """
        try:
            config = open("config.txt", "r")
            SERIAL_PORT = config.readline().decode('utf-8')
            SERIAL_BAUDRATE = toInt(config.readline().decode('utf-8'))
        except:
            print("Could not read config")

    def add_motors_servos(self):
        """
        Parses each line into a list of raw information of each motor/servo.
        Adds the information into motors_servos_dict.
        """
        line = self.read_serial_line()
        while(not re.search("^starting", line)):
            a_servo = parse_setup_input(line)
            motors_servos_dict.append(a_servo)
            line = self.read_serial_line()
    
    @classmethod
    def parse_setup_input(line):
        """
        The setup follows the format of name, motor/servo, several words,
        then the pin. This scrapes the necessary information and returns
        it as a tuple.
        """
        contents = line.split(" ")
        servo_name = contents[0]  # motor/servo name should be first in list
        a_servo = contents[1]     # motor/servo should be second in list
        a_pin = contents[5]       # pin should be sixth in list
        print(servo_name, end= " ")
        print("done")
        return (servo_name, a_servo, a_pin)


    def get_motor_servos(self):
        """
        Returns the list of raw information of the motors/servos.
        """
        return motors_servos_dict

    def complete_setup(self):
        """
        Waits for the setup to be completed. Once it is, marks the port as
        ready to read.
        """
        line = self.read_serial_line()
        while(line != "setup completed"):
            continue

    def parse_setup_input(line):
        """
        The setup follows the format of name, motor/servo, several words,
        then the pin. This scrapes the necessary information and returns
        it as a tuple.
        """
        contents = line.split(" ")
        servo_name = contents[0]  # motor/servo name should be first in list
        a_servo = contents[1]     # motor/servo should be second in list
        a_pin = contents[5]       # pin should be sixth in list
        return (servo_name, a_servo, a_pin)

    def read_serial_line(self):
        """
        The serial returns a line in utf-8 format, so it must be decoded
        to become more readable.
        """
        ser.readline().decode('utf-8')

    def close_serial(self):
        """
        Must close the serial when quitting the application.
        """
        ser.close()

    def get_port(self):
        return SERIAL_PORT()

    def get_baudrate(self):
        return SERIAL_BAUDRATE

    def is_ready_to_read(self):
        """
        Checks whether setup is finished or not.
        """
        return ready_to_read

    def __str__(self):
        return '{self.SERIAL_PORT} | {self.SERIAL_BAUDRATE}'.format(self=self)
