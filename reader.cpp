#include "mbed.h"
#include "MFRC522.h"
#include "reader.h"
#include <string>

// Blinking rate in milliseconds
#define SLEEP_RATE     300ms
extern char publishMQTT[256]; //shared memory with the string to be published

void reader(void){
    
    MFRC522    RfChip   (D11, D12, D13, D10, D8);
     // Init. RC522 Chip
    RfChip.PCD_Init();

    printf("RF chip initailzed\n");
  
    while (true) {
 
        // Look for new cards
        if ( ! RfChip.PICC_IsNewCardPresent())
        {
            ThisThread::sleep_for(SLEEP_RATE);
            continue;
        }
        // Select one of the cards
        if ( ! RfChip.PICC_ReadCardSerial())
        {
            ThisThread::sleep_for(SLEEP_RATE);
            continue;
        }
        
        // Print Card UID
        printf("Card UID: ");
        char sttt[RfChip.uid.size*4 +2 ];
        sprintf(sttt, "");
        for (uint8_t i = 0; i < RfChip.uid.size; i++)
        {
            printf("%X02", RfChip.uid.uidByte[i]);
            sprintf(sttt, "%s %X02",sttt, RfChip.uid.uidByte[i]);
        }

        printf("\nUID: %s \n\r", sttt);

        // Print Card type
        uint8_t piccType = RfChip.PICC_GetType(RfChip.uid.sak);
        printf("PICC Type: %s \n\r", RfChip.PICC_GetTypeName(piccType));

        sprintf(publishMQTT, "{\"UID\":\"%s\",\"type\":%s}", sttt, RfChip.PICC_GetTypeName(piccType));

        ThisThread::sleep_for(SLEEP_RATE * 3);
    }
}
