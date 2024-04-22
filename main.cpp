#include <iostream>
#include <random>
#include <climits>
#include <string>
#include <cctype>

using namespace std;

const unsigned int NUMBER_OF_EXPERIMENTS = 1000000;
const unsigned short NUMBER_OF_DOORS = 3;
enum DoorStates
{
    ClosedEmpty,
    OpenEmpty,
    HiddenPrize
};

void SetSeed();
void GetRandomSeed(unsigned int &seed);
double DoExperiment(bool changeDoor, double previousResult = NAN);
string GetPercentageResult(double success, double previousSuccessRate = NAN);

int main()
{
    if constexpr (NUMBER_OF_DOORS < 3)
    {
        cerr << "\033[1;31mFATAL: NUMBER_OF_DOORS needs to be at least 3. Exiting...\033[1;0m";
        return -1;
    }

    // Get seed from user or generate a random one.
    SetSeed();

    // Try without changing door.
    double result = DoExperiment(false);

    // Try changing door.
    DoExperiment(true, result);

    cout << endl << "Press <Enter> to exit";
    getchar();

    return 0;
}

void SetSeed()
{
    // Decide seed.
    unsigned int seed;
    try
    {
        cout << "Enter a seed. Press <Enter> to use random seed: ";
        string input;
        getline(cin, input);

        if (!cin)
        {
            GetRandomSeed(seed);
            cin.clear();
        }
        else
        {
            seed = stoul(input, nullptr, 10);
            cout << "Using seed: " << seed << endl;
        }
    }
    catch (exception e)
    {
        GetRandomSeed(seed);
    }

    cout << "Doing " << NUMBER_OF_EXPERIMENTS << " experiments with " << NUMBER_OF_DOORS << " doors. " << endl;
    // Set seed.
    srand(seed);
}

void GetRandomSeed(unsigned int &seed)
{
    random_device rd;  // a seed source for the random number engine
    mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    uniform_int_distribution<> distrib(0, UINT_MAX);
    seed = (unsigned int)distrib(gen);

    cout << "\033[1;33mUsing seed: " << seed << " instead.\033[1;0m" << endl;
}

class Experiment
{
public:
    Experiment()
    {
        // Place the hidden prize behind a random door.
        int index = rand() % NUMBER_OF_DOORS;
        for (int i = 0; i < NUMBER_OF_DOORS; i++)
        {
            if (i == index)
            {
                mDoors[i] = HiddenPrize;
            }
            else
            {
                mDoors[i] = ClosedEmpty;
            }
        }
    }

    // Claims a door. Host opens an empty door and ask if you want to switch.
    int ClaimRandomDoor()
    {
        // Claims a random door.
        int contestantsChoice = rand() % NUMBER_OF_DOORS;

        // Host open a random empty door that the contestant has not chosen
        while (true)
        {
            int toOpen = rand() % NUMBER_OF_DOORS;
            if (toOpen != contestantsChoice && mDoors[toOpen] == ClosedEmpty)
            {
                mDoors[toOpen] = OpenEmpty;
                break;
            }
        }

        return contestantsChoice;
    }

    int ChangeDoor(int currentlyClaimed)
    {
        // Change door to the a random closed and unclaimed door.
        while (true)
        {
            int toOpen = rand() % NUMBER_OF_DOORS;
            if (toOpen != currentlyClaimed && mDoors[toOpen] != OpenEmpty)
            {
                return toOpen;
            }
        }

        // Should not happen
        return -1;
    }

    DoorStates OpenDoor(int doorToOpen)
    {
        return mDoors[doorToOpen];
    }

private:
    DoorStates mDoors[NUMBER_OF_DOORS];
};

double DoExperiment(bool changeDoor, double previousResult)
{
    int success = 0;
    if (changeDoor)
    {
        cout << "Changing door:\t\t";
    }
    else
    {
        cout << "Without changing door:\t";
    }

    for (unsigned int i = 0; i < NUMBER_OF_EXPERIMENTS; i++)
    {
        Experiment experiment = Experiment();
        // Claim a random door
        int door = experiment.ClaimRandomDoor();

        // Switch?
        if (changeDoor)
        {
            door = experiment.ChangeDoor(door);
        }

        DoorStates result = experiment.OpenDoor(door);

        // If we found the hidden prize add it to our success count!
        success += (result == HiddenPrize);
    }

    // Fixes division by 0 if contestant is really unlucky.
    if (success == 0)
    {
        success++;
    }

    double successRate = (double)success / NUMBER_OF_EXPERIMENTS * 100;
    cout << "Won " << success << " times out of " << NUMBER_OF_EXPERIMENTS << "\t" << GetPercentageResult(successRate, previousResult) << endl;

    return successRate;
}

string GetPercentageResult(double successRate, double previousSuccessRate)
{
    string color;
    if (!isnan(previousSuccessRate))
    {
        if (successRate >= previousSuccessRate)
        {
            color = "\033[1;32m";
        }
        else
        {
            color = "\033[1;31m";
        }
    }

    return color + to_string(successRate) + "%\033[1;0m";
}
