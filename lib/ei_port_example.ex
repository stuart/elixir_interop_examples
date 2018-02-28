defmodule EiPortExample do
def start() do
	Process.spawn(__MODULE__, :init, ["./priv/bin/polynomial"], [])
end

def stop do
	send(__MODULE__, :stop)
end

def polynomial(coeffs, x) do
	send(__MODULE__,{:call, self(), {coeffs, x}})
	receive do
		{__MODULE__, result} -> result
	end
end

def init(external_program) do
	:erlang.register(__MODULE__, self())
	Process.flag(:trap_exit, true)
	port = Port.open({:spawn, external_program}, [{:packet, 2}, :binary])
	loop(port)
end

def loop(port) do
	receive do
		{:call, caller, msg} ->
			Port.command(port, :erlang.term_to_binary(msg))
			receive do
				{_port, {:data, data}} ->
					send(caller, {__MODULE__, :erlang.binary_to_term(data)})
			end
			loop(port)
		:stop ->
			send(port, {self(), :close})
			receive do
				{_port, :closed} ->
					exit(:normal)
			end
	  {:EXIT, _port, _reason} ->
			exit(:port_terminated)
		end
	end
end
