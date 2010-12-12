#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>

#include "../shapes/Area.hpp"

class TestAreaShape : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestAreaShape );
        CPPUNIT_TEST( testLinear );
        CPPUNIT_TEST( testArea );
    CPPUNIT_TEST_SUITE_END();

private:
    Area a1;
    Area a2;
    Area a3;

public:
    void setUp()
    {
        // counter clock wise
        a1.addPoint(QPointF(0,0));
        a1.addPoint(QPointF(2,0));
        a1.addPoint(QPointF(2,2));
        a1.addPoint(QPointF(0,2));
        a1.addPoint(QPointF(0,0));

        // clockwise
        a2.addPoint(QPointF(0,0));
        a2.addPoint(QPointF(0,2));
        a2.addPoint(QPointF(2,2));
        a2.addPoint(QPointF(2,0));

        a3.addPoint(QPointF(5,5));
        a3.addPoint(QPointF(6,5));
        a3.addPoint(QPointF(6,6));
        a3.addPoint(QPointF(5,6));
        a3.addPoint(QPointF(5,5));
        a3.linkShape(&a2);
    }

    void tearDown()
    {
    }

    void testLinear()
    {
        static double tol = 0.0001;
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 8.0f, a1.length(), tol );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 8.0f, a2.length(), tol );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 8.0f + 4.0f, a3.length(), tol );
    }

    void testArea()
    {
        static double tol = 0.0001;
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 4.0f, a1.area(), tol );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 4.0f, a2.area(), tol );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 4.0f + 1.0f, a3.area(), tol );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestAreaShape );

int main( int argc, char **argv)
{
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry& registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );

    bool wasSucessful = runner.run();
    return wasSucessful ? EXIT_SUCCESS : EXIT_FAILURE;
}

