#define PACKET_SIZE 20
#define BITS_PER_BYTE 8
#define MAX_BUFFER_SIZE 30
#define LOWER_4BIT_MASK 0x0F

struct BlePacket {
	/* Start packet header */
	// Highest 4 bits: packet type ID, lowest 4 bits: number of padding bytes
	byte metadata;
	uint16_t seqNum;
	/* End packet header */
	/* Start packet body */
	// 16-bytes of data, e.g. accelerometer data
	byte data[16];
	/* End packet body */
	/* Start footer */
	byte checksum;
	/* End footer */
};

enum packetIds {
	HELLO = 0,
	ACK = 1,
	P1_IMU = 2,
	P1_IR_RECV = 3,
	P1_IR_TRANS = 4,
	P2_IMU = 5,
	P2_IR_RECV = 6,
	P2_IR_TRANS = 7,
	GAME_STAT = 8
};

template <typename T> class CircularBuffer {
private:
  T elements[MAX_BUFFER_SIZE];
  int head;
  int tail;
  int length;

  int getLength() {
    return this->length;
  }

  void setLength(int length) {
    this->length = length;
  }

public:
  CircularBuffer() {
    this->length = 0;
    this->head = 0;
    this->tail = 0;
  }

  bool push_back(T element) {
    if (isFull()) {
      return false;
    }
    this->tail = (this->tail + 1) % MAX_BUFFER_SIZE;
    this->elements[this->tail] = element;
    this->setLength(this->getLength() + 1);
    return true;
  }

  T pop_front() {
    if (isEmpty()) {
      return NULL;
    }
    T current = this->elements[this->head];
    this->head = (this->head - 1) % MAX_BUFFER_SIZE;
    this->elements[this->head] = NULL;
    this->setLength(this->getLength() - 1);
    return current;
  }

  bool isEmpty() {
    return this->getLength() == 0;
  }

  bool isFull() {
    return this->getLength() == MAX_BUFFER_SIZE;
  }

  int size() {
    return this->getLength();
  }
};

bool isPacketValid(BlePacket &packet) {
  // TODO: Implement actual checks
  return true;
}

void convertBytesToPacket(String &dataBuffer, BlePacket &packet) {
  packet.metadata = dataBuffer.charAt(0);
  packet.seqNum = dataBuffer.charAt(1) + (dataBuffer.charAt(2) << BITS_PER_BYTE);
  byte index = 3;
  for (auto &dataByte : packet.data) {
    dataByte = dataBuffer.charAt(index);
    index += 1;
  }
  packet.checksum = dataBuffer.charAt(PACKET_SIZE - 1);
}