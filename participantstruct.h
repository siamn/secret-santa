typedef struct Participant
{
    int connIndex; // index of connection for stored participant
    char *name;
    char *giftee;
} P;

typedef struct ParticipantsList
{
    P *arr;
    int length;
} PList;