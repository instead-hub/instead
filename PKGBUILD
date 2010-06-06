# Contributor: Peter Kosyh <p.kosyhgmail.com>
pkgname=instead
pkgver=1.2.0
pkgrel=1
pkgdesc="instead quest interpreter"
arch=('i686' 'x86_64')
url="http://instead.googlecode.com/"
license=('GPL')

depends=('sdl' 'sdl_image' 'sdl_mixer' 'sdl_ttf' 'lua')
makedepends=( 'pkgconfig' )

source=(http://instead.googlecode.com/files/instead_$pkgver.tar.gz)
md5sums=(e917080c1627f6e72b0f750fc485e49c)
optdepends=('instead-launcher: install and update INSTEAD games from net')

build() {
cd $startdir/src/instead-$pkgver
echo "2" | ./configure.sh
make PREFIX=/usr || return 1.
make DESTDIR=${startdir}/pkg PREFIX=/usr install
}
