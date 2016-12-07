elementclass Router { IP $ip |
filter::DesIPFilter(IP $ip)
routing::RouteTable()
input[0] -> routing

input[1] -> IPHeaderCheck
         -> [0] filter [0]
         -> Strip(20)
         -> [1] output

filter[1] -> routing

routing[0] -> [0] output
}
