bool RectCollision( point_t aPoint, objSize_t aSize, point_t bPoint, objSize_t bSize )
{
	if( ( aPoint.x - aSize.w/2 <= bPoint.x + bSize.w/2
	and   aPoint.x + aSize.w/2 >= bPoint.x - bSize.w/2 )
	and ( aPoint.y - aSize.h/2 <= bPoint.y + bSize.h/2
	and   aPoint.y + aSize.h/2 >= bPoint.y - bSize.h/2 ) )
		return true;
	else
		return false;
}
int Distance( point_t a, point_t b )
{
	int ans = (b.y-a.y)*(b.y-a.y) + (b.x-a.x)*(b.x-a.x);
	ans = sqrt( ans );
	return ans;
}
