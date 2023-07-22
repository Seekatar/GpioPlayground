#pragma once

/// <summary>
/// Smoothing
/// </summary>
class Smoothing
{
private:
    static const int numReadings = 10;

    int readings[numReadings]; // the readings from the analog input
    int readIndex = 0;         // the index of the current reading
    int total = 0;             // the running total
    int average = 0;           // the average

public:
    Smoothing();
    int Smooth(int);
};