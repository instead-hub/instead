/* Taken from Noise1234, ported to C by Peter Kosyh
 Author: Stefan Gustavson (stegu@itn.liu.se)

 This library is public domain software, released by the author
 into the public domain in February 2011. You may do anything
 you like with it. You may even remove all attributions,
 but of course I'd appreciate it if you kept my name somewhere.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.
*/

/** 1D, 2D, 3D and 4D float Perlin noise, SL "noise()"
 */

extern float noise1( float x );
extern float noise2( float x, float y );
extern float noise3( float x, float y, float z );
extern float noise4( float x, float y, float z, float w );

/** 1D, 2D, 3D and 4D float Perlin periodic noise, SL "pnoise()"
 */
extern float pnoise1( float x, int px );
extern float pnoise2( float x, float y, int px, int py );
extern float pnoise3( float x, float y, float z, int px, int py, int pz );
extern float pnoise4( float x, float y, float z, float w,
                              int px, int py, int pz, int pw );
