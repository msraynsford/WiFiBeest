#ifndef Limits_H
#define Limits_H

#define MIN 500
#define MAX 2500

class limits_t {
  public:
    uint16_t min = 960;
    uint16_t max = 2000;
    uint16_t zero = (MIN + MAX) / 2;

    limits_t() {}
    
    bool checkLimits() {
      return ((min >= MIN) && (min <= MAX) &&
              (zero >= MIN) && (zero <= MAX) &&
              (max >= MIN) && (max <= MAX)); 
    }

  private:
};

#endif
