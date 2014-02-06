// bt-migrate, torrent state migration tool
// Copyright (C) 2014 Mike Gelfand <mikedld@mikedld.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "TorrentStateStoreFactory.h"

#include "DelugeStateStore.h"
#include "Exception.h"
#include "rTorrentStateStore.h"
#include "TransmissionStateStore.h"
#include "uTorrentStateStore.h"

#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

TorrentStateStoreFactory::TorrentStateStoreFactory()
{
    //
}

ITorrentStateStorePtr TorrentStateStoreFactory::CreateForClient(TorrentClient::Enum client) const
{
    switch (client)
    {
    case TorrentClient::Deluge:
        return ITorrentStateStorePtr(new DelugeStateStore());
    case TorrentClient::rTorrent:
        return ITorrentStateStorePtr(new rTorrentStateStore());
    case TorrentClient::Transmission:
        return ITorrentStateStorePtr(new TransmissionStateStore());
    case TorrentClient::uTorrent:
        return ITorrentStateStorePtr(new uTorrentStateStore());
    }

    throw Exception("Unknown torrent client");
}

ITorrentStateStorePtr TorrentStateStoreFactory::GuessByDataDir(fs::path const& dataDir, Intention::Enum intention) const
{
    ITorrentStateStorePtr result;
    for (int client = TorrentClient::FirstClient; client <= TorrentClient::LastClient; ++client)
    {
        ITorrentStateStorePtr store = CreateForClient(static_cast<TorrentClient::Enum>(client));
        if (store->IsValidDataDir(dataDir, intention))
        {
            if (result != nullptr)
            {
                throw Exception("More than one torrent client matched configuration directory");
            }

            result = std::move(store);
        }
    }

    if (result == nullptr)
    {
        throw Exception("No torrent client matched configuration directory");
    }

    return std::move(result);
}
