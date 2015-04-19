<?

class CIDR {

// Expands a CIDR notation address into ranges.
//
// Thanks to M. Garkier for information and code bits on some important issues with this.
//
// example usage:
// $ip_range=expand_CIDR("111.111.111.0/24","SHORT","IP");
// print_r($ip_range);
//
// Author: mkr@binarywerks.dk
// 2-Sep-2003
//
// Fixed flow control bug 12-Mar-2004, that resulted in excessive memory consumption, if invalid ranges was given.
// Added parameters for controlling output - cleaned code.
// Fixed precedence bug in IP validation - Thanks to selt!

function expand_CIDR($ip, $return_type="SHORT", $return_format="LONG")
{
    // validate IP address
    $num    ="(1?\d\d|2[0-4]\d|25[0-5]|[0-9])";
    $range    ="(1\d|2\d|3[0-2]|[1-9])";

    if(!preg_match("/^$num\.$num\.$num\.$num\/$range$/",$ip))
    {
        return "IP wrong format\n";
    }

    // Separate CIDR structure into network-IP and netmask
    $ip_arr = explode("/",$ip);

    // Calculate number of hosts in the subnet
    $mask_bits  = $ip_arr[1];
    if($mask_bits > 31 || $mask_bits < 0)
    {
        return "Nonsense mask\n";
    }

    $host_bits  = 32-$mask_bits;
    $num_hosts  = pow(2,$host_bits)-1;

    // Netmask in decimal for use later: (hack around PHP always using signed ints)
    $netmask    = ip2long("255.255.255.255")-$num_hosts;

    // Calculate start and end
    // Store IP-addresses internally as longs, to ease compare of two
    // addresses in a sorted structure.
    $ip_start  = ip2long($ip_arr[0]);

/*
    if($ip_start != ( $ip_start & $netmask ))
    {
        return "WARNING: Address $ip not on network boundary\n";
    }
*/
    $ip_start = $ip_start & $netmask;

    $ip_end = $ip_start + $num_hosts;

    switch($return_type)
    {
        case "FULL":
            switch($return_format)
            {
                case 'LONG':
                    for($i=0;$i<=$num_hosts;$i++)
                    {
                        $ip_range[] = $ip_start+$i;
                    }
                break;

                case 'IP':
                    for($i=0;$i<=$num_hosts;$i++)
                    {
                        $ip_range[] = long2ip($ip_start+$i);
                    }
                break;

                default:
                    return "Unknown return format: $return_format\n";
                break;
            }

        break;

        case "SHORT":
            switch($return_format)
            {
                case 'LONG':
                    $ip_range[]=$ip_start;
                    $ip_range[]=$ip_end;
                break;

                case 'IP':
                    $ip_range[]=long2ip($ip_start);
                    $ip_range[]=long2ip($ip_end);
                break;

                default:
                    return "Unknown return format: $return_format\n";
                break;
            }
        break;

        default:
            return "Unknown return type: $return_type\n";
        break;
    }

    return $ip_range;
}


}

?>

