defmodule PortExample do
	use GenServer
	def start_link() do
		GenServer.start_link(__MODULE__, [], name: __MODULE__)
	end


	@doc """
	  Evaluates the n th Bessel function at point x.

		iex> PortExample.bessel(1, 2.0)
		0.5767248077568734

	"""
	def bessel(n, x) do
		GenServer.call(__MODULE__, {:bessel, n, x})
		receive do
			{:bessel, res} -> res
			msg -> {:error, msg}
		end
	end

	def init([]) do
		Process.flag(:trap_exit, true)
		port = Port.open({:spawn, "./priv/bin/bessel", }, [:binary])
		{:ok, {port, %{}, 0}}
	end

	def handle_call({:bessel, n, x}, {pid, _ref}, {port, jobs, jobnumber}) do
		send(port, {self(), {:command, encode(jobnumber,n,x)}})
		{:reply, :ok, {port, Map.put(jobs, jobnumber, pid), jobnumber + 1}}
	end

	def handle_info({port, {:data, msg}}, {port, jobs, jobnumber}) do
		{ref, result} = decode(msg)
		if(jobs[ref]) do
			send jobs[ref], {:bessel, result}
		end
		{:noreply, {port, Map.delete(jobs, ref), jobnumber}}
	end

	def handle_info({:EXIT, port ,reason}, state) do
		IO.puts "Port #{inspect port} has exited."
		{:stop, reason, state}
	end

	def handle_info(msg, state) do
		IO.puts "Unhandled message: #{inspect msg}"
		{:noreply, state}
	end

	defp encode(ref, n, x) do
		<<12::16, ref::integer-native-size(16), n::integer-native-size(16), x::float-native-size(64)>>
	end

	defp decode(<<_len::16, ref::integer-native-size(16), result::float-native-size(64)>>) do
		{ref, result}
	end
end
