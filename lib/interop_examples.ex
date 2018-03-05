defmodule InteropExamples do
	use Application

	def start(_type, _args) do
		children = [
			%{ id: PortExample,
		 		 start: {PortExample, :start_link, []}},
		]

		PortDriverExample.start()
		EiPortExample.start()

		{:ok, _pid} = Supervisor.start_link(children, strategy: :one_for_one)
	end
end
