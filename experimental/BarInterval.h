#ifndef BARINTERVAL
#define BARINTERVAL

enum BarType {
  TickBar = 0,
  Minute = 1,
  Daily = 2,
  Weekly = 3,
  Monthly =  4,
  Point_and_Figure = 5,
  Reserved1 = 6,
  Reserved2 = 7,
  Kagi = 8,
  Kase = 9,
  LineBreak = 10,
  Momentum = 11,
  Range = 12,
  Renko = 13,
  Second = 14
};

enum BarInterval {
  minute_1,
  minute_5,
  minute_60,
};

class Bar {

    static QString get_interval_name(const BarInterval &interval) {

    }

};

#endif // BARINTERVAL
