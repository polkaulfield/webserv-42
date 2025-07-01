/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uploadedFile.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arcebria <arcebria@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 17:51:45 by arcebria          #+#    #+#             */
/*   Updated: 2025/07/01 19:58:38 by arcebria         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uploadedFile.hpp"

void ClientRequest::_parseContentType(std::string const& request) {
	size_t	ctPos = request.find("Content-Type:");
	if (ctPos == std::string::npos)
		return;
	size_t	end = request.find("\r\n", ctPos);
	if (end == std::string::npos)
		return;
	size_t	start = ctPos + 13;
	_contentType = request.substr(start, end - start);

}
