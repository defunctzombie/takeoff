/*
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

static double tol = 0.0001;
CPPUNIT_ASSERT_DOUBLES_EQUAL( 8.0f, a1.length(), tol );
CPPUNIT_ASSERT_DOUBLES_EQUAL( 8.0f, a2.length(), tol );
CPPUNIT_ASSERT_DOUBLES_EQUAL( 8.0f + 4.0f, a3.length(), tol );

static double tol = 0.0001;
CPPUNIT_ASSERT_DOUBLES_EQUAL( 4.0f, a1.area(), tol );
CPPUNIT_ASSERT_DOUBLES_EQUAL( 4.0f, a2.area(), tol );
CPPUNIT_ASSERT_DOUBLES_EQUAL( 4.0f + 1.0f, a3.area(), tol );

//length - Area shape
    const int s = _points.size();
    float length = 0;

    //add the length from first to last for area
    //closes the area
    const QPointF p1 = _points.first();
    const QPointF p2 = _points.last();

    length += QLineF(p1, p2).length();

    for (int i=1; i <s; ++i)
    {
        const QPointF p1 = _points[i-1];
        const QPointF p2 = _points[i];

        length += QLineF(p1, p2).length();
    }

    if (_sibling)
        length += _sibling->length();

    return length;


// area - Area shape
    ///http://local.wasp.uwa.edu.au/~pbourke/geometry/polyarea/

    float area = 0;

    if (_points.size() < 3)
        return 0;

    const int s = _points.size();
    for (int i=0; i <s; ++i)
    {
        const QPointF& p1 = _points[i];
        const QPointF& p2 = _points[(i+1) % s];

        area += (p1.x()*p2.y() - p2.x()*p1.y());
    }

    area = qAbs(area) * .5;
	if (_sibling)
        area += _sibling->area();

    return area;

// length - line shape
    const int s = _points.size();
    float length = 0;

    for (int i=1; i <s; ++i)
    {
        const QPointF p1 = _points[i-1];
        const QPointF p2 = _points[i];

        length += QLineF(p1, p2).length();
    }

    if (_sibling)
        length += _sibling->length();

    return length;

    */
