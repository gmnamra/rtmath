      program C6
c     ..................................................................
c     .  Light Scattering by Particles: Computational Methods          .
c     .  by P.W. Barber and S.C. Hill                                  .
c     .  copyright (c) 1990 by World Scientific Publishing Co Pte Ltd  .
c     .                                                                .
c     .  equation numbers in columns 73-80 are references to the text  .
c     ..................................................................
c     .............................................................
c     .  calculate the internal intensity on a central line or a  . 
c     .    rectangular grid for an infinite cylinder illuminated  .
c     .    at normal incidence                                    .
c     .  inputs: ip = polarization par (TM) or perp (TE)          .
c     .          x = size parameter (ka)                          .
c     .          cm = complex index of refraction, (real,imag)    .
c     .               (imag is positive for absorption)           .
c     .          npnts = number of points (multiple of 4)         .
c     .          idim = graph dimension (2 or 3)                  .
c     .                                                           .
c     .  dimension of arrays amat(*) and by(*):                   .
c     .    nc = int(x+4.05*x**.3333+2.0), e.g., for x = 200,      .
c     .    nc = 225                                               .
c     .                                                           .
c     .  dimension of array bj(*):                                .
c     .    nst+1 = nc+int((101.0+x)**.5)+1, e.g., for x = 200,    .
c     .    nst+1 = 243                                            .
c     .                                                           .
c     .  dimension of array aj(*) is determined by z = cm*x:      .
c     .    nst+1 = int(abs(z)+4.05*abs(z)**.3333+2.0              .
c     .                +(101.0+abs(z))**.5)+1,                    .
c     .    e.g., for x = 200, cm = 1.5, nst+1 = 350               .
c     .                                                           .
c     .  arrays are set for a maximum size parameter of 200       .
c     .............................................................
      complex cf(225),ci,cm,cmx,erho,ephi,ez,aj(350)
      common /cfcom/ cf
      dimension ac(1000),phi(2)
      pi = 3.14159265358979
      ci = (0.0,1.0)
      write(6,100)
      write(6,*) 'enter polarization, size parameter, mr, mi'
      read(5,*) ip,x,cmr,cmi
c     .........................................
c     .  set the complex index of refraction  .
c     .    for an exp(-iwt) time variation    .
c     .........................................
      cm = cmplx(cmr,cmi)
      write(6,110)
      write(6,*) 'enter npnts, idim'
      read(5,*) npnts,idim
      open(unit=9,file='c6.dat')
      rewind 9
c     ..................................
c     .  set npnts to a multiple of 4  .
c     ..................................
      npnts = 4*(npnts/4)
      npnt = npnts/2
      npm = npnts+1
      if(idim.eq.2) then
        ngm = 2
        write(6,120) npnts
      else
        ngm = npnts/4+1
        write(6,130) ngm,npnts
        do 10 i = 1,npnts
          ac(i) = 0.0
10      continue
        write(9,140) (ac(j),j=1,npnts)
      end if
      dltx = 2.0/real(npnts-1)
      dlty = 2.0/real(npnt)
      call clndr(x,cm,ip,nc)
c     ....................................
c     .  set starting y grid value (yg)  .
c     .  enter loop to vary y values     .
c     ....................................
      yg = real(2-idim)*(1.0-dlty)
      do 70 ig = 2,ngm
c     .....................................................
c     .  set starting x grid value (xg)                   .
c     .  ns = first grid point on or inside the cylinder  .
c     .  enter loop to vary x values                      .
c     .....................................................
        ns = int((1.0-sqrt(1.0-yg**2))/dltx)+2
        if(ig.eq.ngm) ns = 1
        xg = -1.0+dltx*real(ns-1)
        do 50 jg = ns,npnt
          ra = sqrt(xg**2+yg**2)
          cmx = ra*cm*x
c     ..............................................
c     .  calculate the number of terms needed for  .
c     .    convergence at each radius: nj .le. nc  .
c     ..............................................
          nj = min(int(abs(cmx)+4.05*abs(cmx)**.3333+2.0),nc)
          call besj(cmx,aj,nj)
          pa = atan(yg/xg)
          phi(1) = pi+pa
          phi(2) = 2.0*pi-pa
c     ...........................................
c     .  phi(1) calculates intensity for x < 0  .
c     .  phi(2) calculates intensity for x > 0  .
c     ...........................................
          do 40 iphi = 1,2
            if(ip.eq.1) then
c     .....................
c     .  TM polarization  .
c     .....................
              ez = cf(1)*aj(1)
              do 20 n = 2,nj
                rn = real(n-1)
                ez = ez+2.0*cf(n)*aj(n)*cos(rn*phi(iphi))*ci**(n-1)     eq 2.33
20            continue
              e2 = real(ez)**2+aimag(ez)**2
            else
c     .....................
c     .  TE polarization  .
c     .....................
              ephi = -cf(1)*(-aj(2))
              erho = 0.0
              do 30 n = 2,nj
                rn = real(n-1)
                ephi = ephi-2.0*cf(n)*(aj(n-1)-aj(n)*rn/cmx)
     1                         *cos(rn*phi(iphi))*ci**(n-1)             eq 2.35
                erho = erho-2.0*cf(n)*rn*(aj(n)/cmx)
     1                         *sin(rn*phi(iphi))*ci**(n-1)             eq 2.34
30            continue
              ephi = ci*ephi
              erho = ci*erho
              e2 = (real(ephi)**2+aimag(ephi)**2
     1             +real(erho)**2+aimag(erho)**2)/abs(cm)**2
            end if
c     ...............................................
c     .  store intensity at (xg,yg) in array ac(*)  .
c     ...............................................
            if(iphi.eq.1) then
              ac(jg) = e2
            else
              ac(npm-jg) = e2
            end if
40        continue
c     .......................
c     .  increment x value  .
c     .......................
          xg = xg+dltx
50      continue
c     ......................................
c     .  write out data for all xg values  .
c     .    (for given yg value)            .
c     ......................................
        if(idim.eq.2) then
          xp = -1.0
          do 60 j = 1,npnts
            write(9,150) xp,ac(j)
            xp = xp+dltx
60        continue
        else
          write(9,140) (ac(j),j=1,npnts)
        end if
c     .......................
c     .  increment y value  .
c     .......................
        yg = yg+dlty
70    continue
      close(unit=9)
      stop
100   format('...........................................',/,
     1       '.  calculate 2D or 3D internal intensity  .',/,
     2       '.  output is written to c6.dat            .',/,
     3       '...........................................',//,
     4       'polarization: TM (1) TE (2)',/,
     5       'size parameter: x',/,
     6       'index of refraction: real,imaginary (+ for absorption)',/)
110   format(/,'number of points: npnts (multiple of 4)',/,
     2         'graph dimension: idim (2 or 3)',/)
120   format(/,'2D dimension:',i4)
130   format(/,'3D grid dimension:',i4,' x',i4)
140   format(e14.6)
150   format(2e14.6)
      end
      subroutine clndr(x,cm,ip,nc)
c     .........................................
c     .  calculate the internal coefficients  .     
c     .    - the order is incremented by one  .
c     .........................................
      complex cf(225),amat(225),an,cm,cmm,b,z,h,hm,aj(350)
      common /cfcom/ cf
      dimension bj(243),by(225)
c     ......................................................
c     .  set the number of terms required for convergence  .
c     ......................................................
      xc = x+4.05*x**.3333+2.0                                          eq 2.18
      nc = int(xc)
      z = cm*x
c     .......................................................
c     .  logarithmic derivative calculation - the order is  .
c     .    incremented by one in the amat(*) array          .
c     .  set the starting order for downward recursion      .
c     .......................................................
      nmx = int(max(xc,abs(z)))+15                                      eq 2.22
      an = 0.0
      do 10 n = nmx,nc,-1
        rn = real(n)
        an = (rn-1.0)/z-1.0/(an+rn/z)
10    continue
      amat(nc) = an
      do 20 n = nc-1,2,-1
        rn = real(n)
        amat(n) = (rn-1.0)/z-1.0/(amat(n+1)+rn/z)                       eq 2.21
20    continue
      amat(1) = -1.0/(amat(2)+1.0/z)
c     ...............................................................
c     .  calculate the Bessel functions - the order is              .
c     .    incremented by one in the bj(*), by(*) and aj(*) arrays  .
c     ...............................................................
      call besh(x,bj,by,nc)
      call besj(z,aj,nc)
c     ............................................
c     .  calculate the zeroeth mode coefficient  .
c     ............................................
      cmm = cm
      if(ip.eq.2) cmm = 1.0/cm
      h = cmplx(bj(1),by(1))
      hm = cmplx(bj(2),by(2))
      cf(1) = (cmm*amat(1)*bj(1)+bj(2))/(cmm*amat(1)*h+hm)
      cf(1) = (bj(1)-cf(1)*h)/aj(1)
c     ............................................
c     .  calculate the higher mode coefficients  .
c     ............................................
      do 30 n = 2,nc
        rn = real(n-1)
        b = cmm*amat(n)+rn/x
        hm = h
        h = cmplx(bj(n),by(n))
        cf(n) = (b*bj(n)-bj(n-1))/(b*h-hm)
        cf(n) = (bj(n)-cf(n)*h)/aj(n)                                   eq 2.5
30    continue
      return
      end
      subroutine besh(x,bj,by,nc)
c     ......................................................
c     .  calculate Bessel functions                        .
c     .  bj = Bessel function of the first kind            .
c     .  by = Bessel function of the second kind           .
c     .  x = real argument                                 . 
c     .  nc = number of orders (0 to nc-1)                 .
c     .  the order of the Bessel functions is incremented  .
c     .    by one in the bj(*) and by(*) arrays            .
c     ......................................................
      dimension bj(243),by(nc)
      gamma = .577215664901533
      pi = 3.14159265358979
c     ................................................
c     .  bj(*) calculation - set the starting order  .
c     .                      for downward recursion  .
c     ................................................
      nst = nc+int((101.0+x)**.5)                                       eq 2.23
c     ....................................................
c     .  set starting values for the two highest orders  .
c     ....................................................
      bj(nst+1) = 0.0
      bj(nst) = 1.0e-35
c     ...................................................
c     .  obtain scaled functions by downward recursion  .
c     ...................................................
      do 10 i = nst-1,1,-1
        ri = real(i)
        bj(i) = 2.0*ri*bj(i+1)/x-bj(i+2)
10    continue
c     ..................................................
c     .  calculate the scale factor and the functions  .
c     ..................................................
      alpha = bj(1)
      do 20 j = 2,nst-2,2
        alpha = alpha+2.0*bj(j+1)                                       eq 2.53
20    continue
      do 30 k = 1,nst-1
        bj(k) = bj(k)/alpha
30    continue
c     ................................................
c     .  by(*) calculation - obtain the zeroeth and  .
c     .                      first order functions   .
c     ................................................
      by(1) = bj(1)*(log(x/2.0)+gamma)
      do 40 m = 1,nst/2-1
        rm = real(m)
        by(1) = by(1)-2.0*((-1.0)**m)*bj(2*m+1)/rm
40    continue
      by(1) = 2.0*by(1)/pi
      by(2) = (bj(2)*by(1)-2.0/(pi*x))/bj(1)
c     ...........................................................
c     .  obtain the higher order functions by upward recursion  .
c     ...........................................................
      do 50 n = 3,nc
        rn = real(n-2)
        by(n) = 2.0*rn*by(n-1)/x-by(n-2)
50    continue
      return
      end
      subroutine besj(z,aj,nc)
c     ......................................................
c     .  calculate Bessel functions of the first kind      .
c     .  aj = Bessel function of the first kind            .
c     .  x = complex argument                              . 
c     .  nc = number of orders (0 to nc-1)                 .
c     .  the order of the Bessel functions is incremented  .
c     .    by one in the aj array                          .
c     ......................................................
      complex aj(350),z,alpha
c     ...................................................
c     .  set the starting order for downward recursion  .
c     ...................................................
      x = abs(z)
      nst =int(x+4.05*x**.3333+2.0+(101.0+x)**.5)                       eq 2.23
c     ....................................................
c     .  set starting values for the two highest orders  .
c     ....................................................
      aj(nst+1) = 0.0
      aj(nst) = 1.0e-35
c     ...................................................
c     .  obtain scaled functions by downward recursion  .
c     ...................................................
      do 10 i = nst-1,1,-1
        ri = real(i)
        aj(i) = 2.0*ri*aj(i+1)/z-aj(i+2)
10    continue
c     ..................................................
c     .  calculate the scale factor and the functions  .
c     ..................................................
      alpha = aj(1)
      do 20 j = 2,nst-2,2
        alpha = alpha+2.0*aj(j+1)                                       eq 2.53
20    continue
      do 30 k = 1,nc
        aj(k) = aj(k)/alpha
30    continue
      return
      end
