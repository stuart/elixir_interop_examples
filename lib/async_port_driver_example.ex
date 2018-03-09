defmodule AsyncPortDriverExample do
  @moduledoc """
		This is based on the example Postgres async driver described in
		http://erlang.org/doc/man/erl_driver.html

		It shows how an async port driver can be used in Elixir.
		The returned data is not in the best format for Elixir, e.g.
		strings are all Erlang style rather than binary.

		Example usage:

			iex(1)> {:ok, db} = AsyncPortDriverExample.connect("host=localhost port=5432 dbname=mydb connect_timeout=10")
			{:ok, #Port<0.3439>}
			iex(2)> AsyncPortDriverExample.select(db, "select * from users")
			{:ok,
			 [
			   ['id', 'email', 'name', 'steam_id', 'crypted_password', 'inserted_at',
			    'updated_at']
			 ]}
			 iex(3)> AsyncPortDriverExample.disconnect(db)
 		 	 :ok

  """
	@drv_connect ?C
	@drv_disconnect ?D
	@drv_select ?S

	def connect(connection_string) do
    case :erl_ddll.load_driver('./priv/bin', 'pg_async') do
      :ok -> :ok
      {:error, :already_loaded} -> :ok
      msg -> exit({:error, :could_not_load_driver, msg})
    end
    port = Port.open({:spawn_driver, "pg_async"}, [:binary])
    :erlang.port_control(port, @drv_connect, connection_string)

		case return_port_data(port) do
      :ok ->
        {:ok, port}
      error ->
        error
    end
	end

	def disconnect(port) do
		:erlang.port_control(port, @drv_disconnect, "")
		r = return_port_data(port)
		Port.close(port)
		r
	end

	def select(port, query) do
		:erlang.port_control(port, @drv_select, query)
		return_port_data(port)
	end

	defp return_port_data(_port) do
		receive do
			{_port, {:data, data}} ->
				:erlang.binary_to_term(data)
		end
	end
end
