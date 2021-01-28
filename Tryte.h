#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <iostream>

class Tryte
{
    private:
    // short to store values from -9,841 to 9,841 (19,683 possible values)
    int16_t m_tryte;
    static const std::string ternary_chars;
    static const std::string septavingt_chars;

    

    public:
    
    /*
    Constructors
    */
    // default constructor, initialise to zero
    Tryte();
    // construct from int
    Tryte(int64_t const x);
    // construct from fixed length strings
    Tryte(std::string tryte_string);
    // construct from septavingt array
    Tryte(std::array<int16_t, 3>& sep_array);
    // construct from ternary array
    Tryte(std::array<int16_t, 9>& tern_array);
    // copy constructor
    Tryte(Tryte const& other);

    /*
    increment/decrement operators
    */

    Tryte& operator++(); // x++
    Tryte operator++(int); // pre-increment; ++x
    Tryte& operator--();
    Tryte operator--(int);
    

    /*
    relational operators
    */
    bool operator==(Tryte const& other);
    bool operator!=(Tryte const& other);
    bool operator<(Tryte const& other);
    bool operator<=(Tryte const& other);
    bool operator>(Tryte const& other);
    bool operator>=(Tryte const& other);

    /*
    tritwise logical operators
    */
    Tryte operator&(Tryte const& other);
    Tryte& operator&=(Tryte const& other);
    Tryte operator|(Tryte const& other);
    Tryte& operator|=(Tryte const& other);
    Tryte operator^(Tryte const& other);
    Tryte& operator^=(Tryte const& other);
    Tryte operator~();

    /*
    tritwise shift operators
    */
    Tryte operator<<(uint16_t const& n);
    Tryte& operator<<=(uint16_t const& n);
    Tryte operator>>(uint16_t const& n);
    Tryte& operator>>=(uint16_t const& n);

    /*
    stream extraction and insertion operators
    */
    friend std::ostream& operator<<(std::ostream& os, Tryte const& tryte);
    friend std::istream& operator>>(std::istream& is, Tryte& tryte);

    /*
    string handling functions

    */
    // convert Tryte into ternary string
    std::string ternary_string() const;
    // convert Tryte into septavingtesmal string
    std::string  septavingt_string() const;

    /*
    Helpful functions
    */
    // convert Tryte into array of -1, 0, 1s
    std::array<int16_t, 9> ternary_array() const;
    // convert Tryte into array of septavingtesmal values (-13 <= x <= 13)
    std::array<int16_t, 3> septavingt_array() const;
    // get integer equivalent of Tryte
    int16_t get_int() const;
    // convert a ternary array into a short int, ready for Tryte
    int16_t static ternary_array_to_int(std::array<int16_t, 9> ternary_array);
    // convert a septavingtesmal array into a short int, ready for Tryte
    int16_t static septavingt_array_to_int(std::array<int16_t, 3> septavingt_array);
    // convert a ternary string into its decimal value
    int16_t static ternary_string_to_int(std::string t_string);
    // convert a septavingtesmal string into its decimal value
    int16_t static septavingt_string_to_int(std::string s_string);
    // convert int into short (for preventing internal overflow)
    int16_t static truncate_int(int64_t const& n);
    // Function to pull integers into the range -13 <= x <= 13.
    // The first digit in the returned array is the 'carry'
    // The second digit in the returned array is the 'remainder'
    // Example: overflow_handle(20) returns {1, -7}, as 20 = 1*27 - 7.
    std::array<int16_t, 2> static carry_handler(int16_t n);

    /*
    arithmetic functions
    */
    // add two Trytes, but disregard carry
    Tryte operator+(Tryte const& other);
    Tryte& operator+=(Tryte const& other);
    // subtract Trytes, ignore carry
    Tryte operator-(Tryte const& other);
    Tryte& operator-=(Tryte const& other);
    // flip sign
    Tryte operator-() const;
    // add two Trytes, tritwise
    static Tryte tritwise_add(Tryte const& t1, Tryte const& t2);
    // multiply two Trytes, tritwise
    static Tryte tritwise_mult(Tryte const& t1, Tryte const& t2);
    // add two Trytes and a carry, and keep the new carry (for Trint ops)
    static std::array<Tryte, 2> add_with_carry(Tryte const& t1, Tryte const& t2, Tryte const& carry);
    // mutliply two Trytes and keep the carry (for Trint ops)
    static std::array<Tryte, 2> mult(Tryte const& t1, Tryte const& t2);
};