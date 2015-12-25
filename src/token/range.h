#ifndef SRC_TOKEN_RANGE_H_
#define SRC_TOKEN_RANGE_H_

struct Range {
  Range(uint16_t i_begin, uint16_t i_end)
    : begin(i_begin), end(i_end) { }
    
  uint16_t begin;
  uint16_t end;
};

#endif  // SRC_TOKEN_RANGE_H_