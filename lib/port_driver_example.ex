defmodule PortDriverExample do
	def start() do
		case :erl_ddll.load_driver('./priv/bin', 'example_drv') do
			:ok -> :ok
			{:error, :already_loaded} -> :ok
			msg -> exit({:error, msg})
		end
		spawn(__MODULE__, :init, ["example_drv"])
		:ok
	end

	def stop do
		send(__MODULE__, :stop)
	end

	@doc """
		Returns "hello world"

		#Example

		iex> PortDriverExample.hello()
		"hello world"

	"""
	def hello() do
		send(__MODULE__,{:call, self()})
		receive do
			{__MODULE__, result} -> result
		end
	end

	@doc "Callback called on spawn"
	def init(shared_lib) do
		:erlang.register(__MODULE__, self())
		Process.flag(:trap_exit, true)
		port = Port.open({:spawn, shared_lib}, [{:packet, 2}, :binary])
		loop(port)
	end

	def loop(port) do
		receive do
			{:call, caller} ->
				Port.command(port, :erlang.term_to_binary(:hello))
				receive do
					{_port, {:data, data}} ->
						send(caller, {__MODULE__, data})
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
