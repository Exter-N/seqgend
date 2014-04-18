<?php

class SeqgenClient
{
	const MAX_IDS_PER_REQUEST = 64;
	
	private $socket;

	public function __construct($port, $host = '127.0.0.1')
	{
		$this->socket = fsockopen('udp://' . $host . ':' . $port);
	}
	public function __destruct()
	{
		fclose($this->socket);
	}

	public function generate($num = 1)
	{
		if ($num <= 0)
			return array();
		elseif ($num <= self::MAX_IDS_PER_REQUEST)
			$raw = $this->generate_raw($num);
		else {
			$raw = '';
			while ($num > 0) {
				$raw .= $this->generate_raw(min(self::MAX_IDS_PER_REQUEST, $num));
				$num -= self::MAX_IDS_PER_REQUEST;
			}
		}
		$num = strlen($raw) >> 3;
		$ids = array();
		for ($i = 0, $o = 0; $i < $num; ++$i, $o += 8)
			$ids[] = self::decode(substr($raw, $o, 8));
		return $ids;
	}

	private static function decode($raw)
	{
		$unpacked = unpack('N2', $raw);
		return self::make64($unpacked[1], $unpacked[2]);
	}
	private static function make64($hi, $lo)
	// http://www.mysqlperformanceblog.com/2007/03/27/integers-in-php-running-with-scissors-and-portability/
	{
	    // on x64, we can just use int
	    if ( ((int)4294967296)!=0 )
	        return (((int)$hi)<<32) + ((int)$lo);
	 
	    // workaround signed/unsigned braindamage on x32
	    $hi = sprintf ( "%u", $hi );
	    $lo = sprintf ( "%u", $lo );
	 
	    // use GMP or bcmath if possible
	    if ( function_exists("gmp_mul") )
	        return gmp_strval ( gmp_add ( gmp_mul ( $hi, "4294967296" ), $lo ) );
	 
	    if ( function_exists("bcmul") )
	        return bcadd ( bcmul ( $hi, "4294967296" ), $lo );
	 
	    // compute everything manually
	    $a = substr ( $hi, 0, -5 );
	    $b = substr ( $hi, -5 );
	    $ac = $a*42949; // hope that float precision is enough
	    $bd = $b*67296;
	    $adbc = $a*67296+$b*42949;
	    $r4 = substr ( $bd, -5 ) +  + substr ( $lo, -5 );
	    $r3 = substr ( $bd, 0, -5 ) + substr ( $adbc, -5 ) + substr ( $lo, 0, -5 );
	    $r2 = substr ( $adbc, 0, -5 ) + substr ( $ac, -5 );
	    $r1 = substr ( $ac, 0, -5 );
	    while ( $r4>100000 ) { $r4-=100000; $r3++; }
	    while ( $r3>100000 ) { $r3-=100000; $r2++; }
	    while ( $r2>100000 ) { $r2-=100000; $r1++; }
	 
	    $r = sprintf ( "%d%05d%05d%05d", $r1, $r2, $r3, $r4 );
	    $l = strlen($r);
	    $i = 0;
	    while ( $r[$i]=="0" && $i<$l-1 )
	        $i++;
	    return substr ( $r, $i );            
	}

	private function generate_raw($num)
	{
		fwrite($this->socket, chr($num));
		return fread($this->socket, $num * 8);
	}
}
