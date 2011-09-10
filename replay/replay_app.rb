require 'rubygems'
require_relative 'replay'

class Integer
    def gigabytes
        self * 1024 * 1024 * 1024
    end

    def megabytes
        self * 1024 * 1024
    end

    def kilobytes
        self * 1024
    end
end

module Replay
    class ReplaySource
        def initialize(opts={})
            buf_size = opts[:buf_size] || 20.gigabytes
            frame_size = opts[:frame_size] || 512.kilobytes
            input = opts[:input] || fail("Cannot have a source with no input")
            file = opts[:file] || fail("Cannot have a source with no file")
            name = opts[:name] || file

            @buffer = ReplayBuffer.new(file, buf_size, frame_size, name)
            @ingest = ReplayIngest.new(input, @buffer)
        end

        def make_shot_now
            @buffer.make_shot(0, ReplayBuffer::END)
        end

        def monitor
            @ingest.monitor
        end
    end

    class ReplayPreview
        def shot
            # workaround for swig not handling output params properly
            sh = ReplayShot.new
            get_shot(sh)
            sh
        end
    end

    class ReplayMultiviewer
        # throw a Ruby-ish frontend on the ugly C++
        def add_source(opts={})
            params = ReplayMultiviewerSourceParams.new
            params.source = opts[:port] || fail("Need some kind of input")
            params.x = opts[:x] || 0
            params.y = opts[:y] || 0

            real_add_source(params)
        end
    end

    # abstraction for what will someday be a config file parser
    class ReplayConfig
        def make_output_adapter
            Replay::create_decklink_output_adapter(0, 0, RawFrame::CbYCrY8422)
        end
    end

    class ReplayShot
        def preview
            # eventually, rig this to some sort of JPEG preview extraction
            ''
        end
    end

    class UniqueList < Hash
        def initialize
            super

            # The id of the next item to be added.
            @next_item_id = 0

            # The item most recently added to the list.
            @last_item_id = nil
        end

        def each_in_order
            keys.sort.each do |i|
                yield self[i]
            end
        end

        def each_in_reverse_order
            keys.sort { |a, b| b <=> a }.each do |i|
                yield self[i]
            end
        end

        def insert(item)
            @last_item_id = @next_item_id
            self[@next_item_id] = item
            @next_item_id += 1
            @last_item_id
        end

    end

    class ReplayEvent < Array
        attr_accessor :name
    end

    class ReplayApp
        def initialize
            @sources = []

            @events = UniqueList.new

            @current_event = nil

            @dpys = FramebufferDisplaySurface.new
            @multiviewer = ReplayMultiviewer.new(@dpys)

            config = ReplayConfig.new # something something something

            @program = ReplayPlayout.new(config.make_output_adapter)
            @preview = ReplayPreview.new

            # wire program and preview into multiviewer
            @multiviewer.add_source(:port => @preview.monitor,
                    :x => 0, :y => 0)
            @multiviewer.add_source(:port => @program.monitor,
                    :x => 960, :y => 0)

            @preview_shot = nil
            @program_shot = nil


            # FIXME hard coded defaults
            @source_pvw_width = 480
            @source_pvw_height = 270
            @mvw = 1920
            @mvx = 0
            @mvy = 540
        end

        def add_source(opts={})
            source = ReplaySource.new(opts)
            @sources << source

            @multiviewer.add_source(:port => source.monitor, 
                    :x => @mvx, :y => @mvy)

            @mvx += @source_pvw_width
            if @mvx >= @mvw
                @mvx = 0
                @mvy += @source_pvw_height
            end
        end

        def source(i)
            @sources[i]
        end

        def start
            @multiviewer.start
        end

        def capture_event
            event = ReplayEvent.new
            @sources.each do |source|
                shot = source.make_shot_now
                event << shot
            end

            id = @events.insert(event)
            event.name = "Event #{id}"

            @current_event = event
            preview_camera(0)

            [event, id]
        end

        def event(id)
            @events[id] || fail("invalid event ID")
        end

        def current_event=(id)
            @current_event = events[id]
        end

        def _pvw
            @preview
        end

        def preview_shot(event_id, source_id)
            @preview.shot = events[id][source_id]
        end

        def preview_camera(source_id)
            if @current_event
                @preview.shot = @current_event[source_id]
            end
        end

        def seek_preview(n_frames)
            @preview.seek(n_frames)
        end

        def roll_start_from_preview
            @preview.mark_in
            @program.shot = @preview.shot
        end

        def roll_stop
            @program.stop
        end

        def roll_speed(num,denom)
            @program.set_speed(num, denom)
        end

        def start_irb
            IRB.start_session(binding())
        end
    end
end