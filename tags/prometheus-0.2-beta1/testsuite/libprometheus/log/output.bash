# Usage: output <text> <retval>
# Echo <text> to stdout & stderr, then return <retval>.
function output {
	echo "stdout: $1"
	echo "stderr: $1" >&2
	return $2
}
