bool RectCollision( SDL_Rect first, SDL_Rect second )
{
	if( ( first.x<=second.x+second.w and first.x+first.w >=second.x ) and ( first.y<=second.y+second.h and first.y+first.h >=second.y ) )
		return true;
	else
		return false;
}
int Distance( int x1, int y1, int x2, int y2 )
{
	int ans = (y2-y1)*(y2-y1) + (x2-x1)*(x2-x1);
	ans = sqrt( ans );
	return ans;
}
