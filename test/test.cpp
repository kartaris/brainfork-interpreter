#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include <fstream>
#include <string>

#include <brainfork_executor.h>


TEST_CASE( "Hello, World!", "[hello-world]" ) {
    std::ofstream out("hello-world.bfk");
    REQUIRE(out);
    out << "+++++ +++++             initialize counter (cell #0) to 10\n"
           "[                       use loop to set 70/100/30/10\n"
           "    > +++++ ++              add  7 to cell #1\n"
           "    > +++++ +++++           add 10 to cell #2\n"
           "    > +++                   add  3 to cell #3\n"
           "    > +                     add  1 to cell #4\n"
           "<<<< -                  decrement counter (cell #0)\n"
           "]\n"
           "> ++ .                  print 'H'\n"
           "> + .                   print 'e'\n"
           "+++++ ++ .              print 'l'\n"
           ".                       print 'l'\n"
           "+++ .                   print 'o'\n"
           "> ++ .                  print ' '\n"
           "<< +++++ +++++ +++++ .  print 'W'\n"
           "> .                     print 'o'\n"
           "+++ .                   print 'r'\n"
           "----- - .               print 'l'\n"
           "----- --- .             print 'd'\n"
           "> + .                   print '!'";
    out.close();
    BrainforkExecutor executor;

    REQUIRE(executor.execute("hello-world.bfk", false));
    auto unoptimizedResult = executor.result();
    REQUIRE(unoptimizedResult == L"Hello World!");

    REQUIRE(executor.execute("hello-world.bfk"));
    auto optimizedResult = executor.result();
    REQUIRE(optimizedResult == L"Hello World!");

    REQUIRE(unoptimizedResult == optimizedResult);
}
