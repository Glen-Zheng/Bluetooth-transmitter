# import os
import uuid

def main():
    
    #send input to pipe
    newUUID = str(uuid.uuid4())
    newUUID = newUUID[:4]

    
    #write generated UUID to file
    fo = open("test.txt", "wb")
    fo.write(newUUID);


if __name__ == "__main__":
    main()